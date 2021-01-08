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
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_WeaponTraceChannel, ECR_Block);
	RootComponent = StaticMesh;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplosionRadialForce"));
	RadialForceComp->SetupAttachment(RootComponent);
	RadialForceComp->Radius = 1000.0f;
	RadialForceComp->ImpulseStrength = 10000.0f;
	
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health"));
}


void ASExplosiveActor::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveActor::OnHealthChanged);
}

void ASExplosiveActor::OnHealthChanged(USHealthComponent* _, int32 HealthDelta)
{
	if (HealthComp->GetCurrentHealthPoints() <= 0 && !bDied)
	{	
		Explode();
		
		bDied = true;
	}
}

void ASExplosiveActor::Explode()
{
	if (!ensureAlways(!bDied))
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
