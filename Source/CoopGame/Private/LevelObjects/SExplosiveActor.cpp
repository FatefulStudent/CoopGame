#include "LevelObjects/SExplosiveActor.h"


#include "CoopGame/CoopGame.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Player/Components/SHealthComponent.h"


ASExplosiveActor::ASExplosiveActor()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionObjectType(ECC_Destructible);
	RootComponent = StaticMesh;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplosionRadialForce"));
	RadialForceComp->Radius = 500.0f;
	RadialForceComp->ImpulseStrength = 500.0f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->SetupAttachment(RootComponent);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health"));
}


void ASExplosiveActor::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveActor::OnHealthChanged);
}

void ASExplosiveActor::OnHealthChanged(USHealthComponent* _, int32 HealthDelta)
{
	if (HealthComp->GetCurrentHealthPoints() <= 0 && !bExploded)
	{	
		Explode();
		
		bExploded = true;
	}
}

void ASExplosiveActor::Explode()
{
	if (!ensureAlways(!bExploded))
		return;

	RadialForceComp->FireImpulse();
	
	PlayCosmeticExplosionEffects();

	UE_LOG(LogTemp, Warning, TEXT("%s: Exploded!"));
}

void ASExplosiveActor::PlayCosmeticExplosionEffects() const
{
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
