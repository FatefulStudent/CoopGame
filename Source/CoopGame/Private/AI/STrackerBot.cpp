#include "AI/STrackerBot.h"
#include "Helpers/NetworkHelper.h"
#include "Common/Components/SHealthComponent.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"
#include "Net/UnrealNetwork.h"
#include "Player/SCharacter.h"


ASTrackerBot::ASTrackerBot()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetCanEverAffectNavigation(false);
	StaticMeshComp->SetSimulatePhysics(true);
	RootComponent = StaticMeshComp;

	PlayerOverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerOverlapSphere"));
	PlayerOverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerOverlapComponent->SetCollisionObjectType(ECC_WorldDynamic);
	PlayerOverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	PlayerOverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PlayerOverlapComponent->SetupAttachment(RootComponent);

	TrackerBotOverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TrackerOverlapSphere"));
	TrackerBotOverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TrackerBotOverlapComponent->SetCollisionObjectType(ECC_WorldDynamic);
	TrackerBotOverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TrackerBotOverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TrackerBotOverlapComponent->SetupAttachment(RootComponent);
	
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health"));
}

void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleHealthChanged);
	PlayerOverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTrackerBot::HandleBeginOverlapOfPlayerOverlapComponent);
	TrackerBotOverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTrackerBot::HandleBeginOverlapOfTrackerBotOverlapComponent);
	TrackerBotOverlapComponent->OnComponentEndOverlap.AddDynamic(this, &ASTrackerBot::HandleEndOverlapOfTrackerBotOverlapComponent);

	// Initial move-to
	if (FNetworkHelper::HasAuthority(this))
		NextPathPoint = CalculateNextPathPoint();
	else
		SetActorTickEnabled(false);
}

void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FNetworkHelper::HasAuthority(this) && !bExploded)
		MoveToTargetByForce();
}

void ASTrackerBot::StartSelfDestructionIfNeeded()
{
	check(FNetworkHelper::HasAuthority(this));
	
	// Start self destruct sequence
	if (!bSelfDestructionStarted)
	{
		bSelfDestructionStarted = true;
		OnRep_bSelfDestructionStarted();
		
		GetWorld()->GetTimerManager().SetTimer(OverlappedWithPlayerSelfHarm_TimerHandle, this, &ASTrackerBot::SelfHarmNearPlayer,
				SelfHarmRate, true, 0.0f);
	}
}

void ASTrackerBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASTrackerBot, bExploded);
	DOREPLIFETIME(ASTrackerBot, bSelfDestructionStarted);
	DOREPLIFETIME(ASTrackerBot, CurrentSwarmDamageModifier);
}

void ASTrackerBot::HandleHealthChanged(USHealthComponent* _, int32 HealthDelta)
{
	if (FNetworkHelper::HasCosmetics(this))
		PlayEffectsOnDamage();
	
	if (FNetworkHelper::HasAuthority(this))
		if (HealthComp->GetCurrentHealthPoints() <= 0)
			Explode();
}

void ASTrackerBot::PlayEffectsOnDamage()
{
	if (DynamicMaterialInstance == nullptr)
		DynamicMaterialInstance = StaticMeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, StaticMeshComp->GetMaterial(0));

	if (DynamicMaterialInstance)
		DynamicMaterialInstance->SetScalarParameterValue(LastTimeDamagedParameterName, GetWorld()->GetTimeSeconds());
}

void ASTrackerBot::HandleBeginOverlapOfPlayerOverlapComponent(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!FNetworkHelper::HasAuthority(this))
		return;

	
	if (OtherActor->IsA<ASCharacter>())
	{
		StartSelfDestructionIfNeeded();
	}
}

void ASTrackerBot::HandleBeginOverlapOfTrackerBotOverlapComponent(UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
	if (!FNetworkHelper::HasAuthority(this))
		return;

	
	if (OtherActor->IsA<ASTrackerBot>() && OtherActor!=this)
	{
		TrackerBotsInRange++;
		RecalculateSwarmDamageModifier();
	}
}

void ASTrackerBot::HandleEndOverlapOfTrackerBotOverlapComponent(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	if (!FNetworkHelper::HasAuthority(this))
		return;
	
	if (OtherActor->IsA<ASTrackerBot>() && OtherActor!=this)
	{
		TrackerBotsInRange--;
		RecalculateSwarmDamageModifier();
	}
}

void ASTrackerBot::RecalculateSwarmDamageModifier()
{
	check(FNetworkHelper::HasAuthority(this));

	const float NewSwarmDamageModifier = TrackerBotsInRange * AdditionalDamageModifierForEachNewBot;
	CurrentSwarmDamageModifier = FMath::Clamp(NewSwarmDamageModifier, 0.0f, MaxSwarmDamageModifier);
	OnRep_CurrentSwarmDamageModifier();
}

FVector ASTrackerBot::CalculateNextPathPoint() const
{
	check(FNetworkHelper::HasAuthority(this));
	
	AActor* GoalActor = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!GoalActor)
		return GetActorLocation();
	 
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(), GetActorLocation(), GoalActor);

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ASTrackerBot::MoveToTargetByForce()
{
	check(FNetworkHelper::HasAuthority(this));
	
	const bool bIsAtGoal = FVector::Distance(GetActorLocation(), NextPathPoint) < AcceptanceRadiusToTarget;
	if (bIsAtGoal)
	{
		NextPathPoint = CalculateNextPathPoint();
	}
	else
	{
		const FVector ForceDirection = (NextPathPoint - GetActorLocation()).GetSafeNormal();
		const FVector Force = ForceDirection * RollingForceStrength;
		StaticMeshComp->AddForce(Force, NAME_None, bAccelChange);
	}
}

void ASTrackerBot::SelfHarmNearPlayer()
{
	check(FNetworkHelper::HasAuthority(this));
	if (!bExploded)
		UGameplayStatics::ApplyDamage(this, SelfHarmDamage, GetController(), this, nullptr);
}

void ASTrackerBot::Explode()
{
	if (bExploded)
		return;

	bExploded = true;
	OnRep_bExploded();
	
	if (FNetworkHelper::HasAuthority(this))
		ApplyDamageAndSelfDestroy();
}

void ASTrackerBot::PlayExplosionEffects() const
{
	check(FNetworkHelper::HasCosmetics(this));
	
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());
	
	UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, GetActorLocation());

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 10, FColor::Red, false, 5.0f, 0, 5);
}

void ASTrackerBot::ApplyDamageAndSelfDestroy()
{
	check(FNetworkHelper::HasAuthority(this));

	const float FinalDamage = ExplosionDamage * (1.0f + CurrentSwarmDamageModifier);
	
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	
	UGameplayStatics::ApplyRadialDamage(this, FinalDamage, GetActorLocation(), ExplosionRadius, ExplosionDamageClass,
                                        IgnoredActors, this, this->GetController(), true);

	SetLifeSpan(1.0f);
}

void ASTrackerBot::OnRep_CurrentSwarmDamageModifier()
{
	const float CurrentPowerLevelClamped = CurrentSwarmDamageModifier / FMath::Max(MaxSwarmDamageModifier, KINDA_SMALL_NUMBER);
	
	if (DynamicMaterialInstance == nullptr)
		DynamicMaterialInstance = StaticMeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, StaticMeshComp->GetMaterial(0));

	if (DynamicMaterialInstance)
		DynamicMaterialInstance->SetScalarParameterValue(PowerLevelParameterName, CurrentPowerLevelClamped);
}

void ASTrackerBot::OnRep_bSelfDestructionStarted() const
{
	if (FNetworkHelper::HasCosmetics(this))
		UGameplayStatics::SpawnSoundAttached(SelfDestructionSequenceStartedSound, StaticMeshComp);
}

void ASTrackerBot::OnRep_bExploded() const
{
	if (FNetworkHelper::HasCosmetics(this))
		PlayExplosionEffects();

	StaticMeshComp->SetSimulatePhysics(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->SetVisibility(false);
}
