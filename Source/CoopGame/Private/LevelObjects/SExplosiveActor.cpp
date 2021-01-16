#include "LevelObjects/SExplosiveActor.h"
#include "Helpers/NetworkHelper.h"
#include "Common/Components/SHealthComponent.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/RadialForceComponent.h"


ASExplosiveActor::ASExplosiveActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
	SetReplicatingMovement(true);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionObjectType(ECC_Destructible);
	RootComponent = StaticMesh;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplosionRadialForce"));
	RadialForceComp->Radius = 500.0f;
	RadialForceComp->ImpulseStrength = 500.0f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->SetupAttachment(RootComponent);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health"));
}


void ASExplosiveActor::BeginPlay()
{
	Super::BeginPlay();

	if (FNetworkHelper::HasAuthority(this))
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveActor::OnHealthChanged);
	}
}

void ASExplosiveActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveActor, bExploded);
}

void ASExplosiveActor::OnHealthChanged(USHealthComponent* _, int32 HealthDelta)
{
	check(FNetworkHelper::HasAuthority(this));
	
	if (HealthComp->GetCurrentHealthPoints() <= 0 && !bExploded)
	{	
		Explode();
		
		bExploded = true;

		OnRep_bExploded();
	}
}

void ASExplosiveActor::Explode()
{
	check(FNetworkHelper::HasAuthority(this));
	
	if (!ensureAlways(!bExploded))
		return;

	RadialForceComp->FireImpulse();

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(
		this,
		ExplosionDamage,
		GetActorLocation(),
		RadialForceComp->Radius,
		ExplosionDamageClass,
		IgnoredActors);
}

void ASExplosiveActor::PlayCosmeticExplosionEffects() const
{
	check(FNetworkHelper::HasCosmetics(this))
	
	if (ExplodedMaterial)
	{
		StaticMesh->SetMaterial(0, ExplodedMaterial);
	}

	if (ExplosionEffect)
	{
		const FRotator RandRotator = UKismetMathLibrary::RandomRotator(false);
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation(), RandRotator);
	}
}

void ASExplosiveActor::OnRep_bExploded() const
{
	const FVector BoostImpulse = FVector::UpVector * RadialForceComp->ImpulseStrength;
	StaticMesh->AddImpulse(BoostImpulse, NAME_None, true);
	
	if (FNetworkHelper::HasCosmetics(this) && bExploded)
		PlayCosmeticExplosionEffects();
}
