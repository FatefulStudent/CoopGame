#include "Weapon/Projectiles/SGrenadeProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ASGrenadeProjectile::ASGrenadeProjectile() 
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetReplicates(true);
	AActor::SetReplicateMovement(true);
	
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASGrenadeProjectile::OnHit);	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
}

void ASGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FTimerHandle ExplodeTimerHandle;
		GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &ASGrenadeProjectile::ExplodeWithEffects, 1.0f);
	}
}

void ASGrenadeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	if (HasAuthority())
	{
		ExplodeWithEffects();
	}
}

void ASGrenadeProjectile::ExplodeWithEffects()
{
	PlayExplosionEffects();
	Explode();
}

void ASGrenadeProjectile::PlayExplosionEffects() const
{
	if (ExplosionEffect)
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());
}

void ASGrenadeProjectile::Explode()
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	IgnoredActors.Add(GetOwner());
	IgnoredActors.Add(GetInstigator());
	
	UGameplayStatics::ApplyRadialDamage(
		this,
		BaseDamage,
		GetActorLocation(),
		DamageRadius,
		DamageType,
		IgnoredActors, 
		this,
		GetInstigatorController(),
		false,
		ECC_Visibility);

	Destroy();
}
