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
#include "Player/SCharacter.h"


ASTrackerBot::ASTrackerBot()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetCanEverAffectNavigation(false);
	StaticMeshComp->SetSimulatePhysics(true);
	RootComponent = StaticMeshComp;

	OverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComponent->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComponent->SetupAttachment(RootComponent);
	

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health"));
}

void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleHealthChanged);
	// Initial move-to
	NextPathPoint = CalculateNextPathPoint();
}

void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToTargetByForce();
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor->IsA<ASCharacter>())
	{
		const bool bSelfDestructTimerAlreadyExists = GetWorld()->GetTimerManager().TimerExists(OverlappedWithPlayerSelfHarm_TimerHandle);

		// Start self destruct sequence
		if (!bSelfDestructTimerAlreadyExists)
			GetWorld()->GetTimerManager().SetTimer(OverlappedWithPlayerSelfHarm_TimerHandle, this, &ASTrackerBot::SelfHarmNearPlayer, SelfHarmRate,
				true, 0.0f);
	}
}

void ASTrackerBot::HandleHealthChanged(USHealthComponent* _, int32 HealthDelta)
{
	PlayEffectsOnDamage();

	if (HealthComp->GetCurrentHealthPoints() <= 0)
		Explode();
	
	UE_LOG(LogTemp, Log, TEXT("%s is damaged. Remaining health is %i"), *GetName(), HealthComp->GetCurrentHealthPoints());
}

void ASTrackerBot::PlayEffectsOnDamage()
{
	if (MaterialForPulseOnDamage == nullptr)
		MaterialForPulseOnDamage = StaticMeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, StaticMeshComp->GetMaterial(0));

	if (MaterialForPulseOnDamage)
		MaterialForPulseOnDamage->SetScalarParameterValue(LastTimeDamagedParameterName, GetWorld()->GetTimeSeconds());
}

FVector ASTrackerBot::CalculateNextPathPoint() const
{
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
	UGameplayStatics::ApplyDamage(this, SelfHarmDamage, GetController(), this, nullptr);
}

void ASTrackerBot::Explode()
{
	if (bExploded)
		return;

	bExploded = true;
	
	if (FNetworkHelper::HasCosmetics(this))
		PlayExplosionEffects();

	if (FNetworkHelper::HasAuthority(this))
		ApplyDamageAndSelfDestroy();
}

void ASTrackerBot::PlayExplosionEffects() const
{
	check(FNetworkHelper::HasCosmetics(this));
	
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 10, FColor::Red, false, 5.0f, 0, 5);
}

void ASTrackerBot::ApplyDamageAndSelfDestroy()
{
	check(FNetworkHelper::HasAuthority(this));
	
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, ExplosionDamageClass,
                                        IgnoredActors, this, this->GetController(), true);
	
	Destroy();
}
