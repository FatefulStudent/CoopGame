#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ASWeapon::ASWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SkeletalMeshComp;
}

void ASWeapon::Fire()
{
	// Trace the world from the owners eyes perspective

	if (AActor* OwnerActor = GetOwner())
	{
		FVector TraceStart;
		FRotator EyeRotation;
		OwnerActor->GetActorEyesViewPoint(TraceStart, EyeRotation);

		const FVector ShotDirection = EyeRotation.Vector();
		const FVector TraceEnd = TraceStart + ShotDirection * 10000.0f;
		
		Shoot(TraceStart, TraceEnd, ShotDirection);
	}
}

void ASWeapon::Shoot(
	const FVector& TraceStart,
	const FVector& TraceEnd,
	const FVector& ShotDirection)
{
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bTraceComplex = true;

	

	FHitResult HitResult;
	const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility);
	if (bBlockingHit)
	{
		if (HasAuthority())
			ApplyDamageToHitActor(ShotDirection, HitResult);

		PlayEffectsOnImpact(HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}

	const FVector& TraceEffectEnd = bBlockingHit ? HitResult.ImpactPoint : TraceEnd;

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 10.0f);

	PlayEffectsOnShoot(TraceEffectEnd);
}

void ASWeapon::ApplyDamageToHitActor(const FVector& ShotDirection, const FHitResult& HitResult)
{
	UGameplayStatics::ApplyPointDamage(
        HitResult.GetActor(),
        20.0,
        ShotDirection,
        HitResult, 
        GetInstigatorController(), 
        this,
        DamageType);
}

void ASWeapon::PlayEffectsOnShoot(const FVector& TraceEffectEnd) const
{
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, SkeletalMeshComp, MuzzleSocketName);

	
	if (TracerEffect)
	{
		const FVector MuzzleLocation = SkeletalMeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TraceParticlesComp =
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		TraceParticlesComp->SetVectorParameter(TraceTargetName, TraceEffectEnd);
	}
}

void ASWeapon::PlayEffectsOnImpact(const FVector& ImpactLocation, const FRotator& ImpactRotation) const
{
	if (ImpactEffect)
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, ImpactLocation, ImpactRotation);
}
