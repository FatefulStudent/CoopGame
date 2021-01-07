#include "Weapon/Parts/SWeaponEffects.h"
#include "Weapon/SWeapon.h"
#include "CoopGame/CoopGame.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

USWeaponEffects::USWeaponEffects()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USWeaponEffects::BeginPlay()
{
	Super::BeginPlay();

	WeaponActor = Cast<ASWeapon>(GetOwner());
	
	if (!WeaponActor)
		ensureAlways(false);
}

void USWeaponEffects::PlayFireEffects(const FVector& TraceEffectEnd) const
{
	PlayCameraShake();
	PlayTraceEffect(TraceEffectEnd);
	PlayMuzzleEffect();
}

void USWeaponEffects::PlayCameraShake() const
{
	if (APlayerController* InstigatorPlayerController = Cast<APlayerController>(WeaponActor->GetInstigatorController()))
	{
		InstigatorPlayerController->ClientPlayCameraShake(CameraShakeClass);
	}
}

void USWeaponEffects::PlayTraceEffect(const FVector& TraceEffectEnd) const
{
	if (TracerEffect)
	{
		const FVector MuzzleLocation = WeaponActor->SkeletalMeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TraceParticlesComp =
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		TraceParticlesComp->SetVectorParameter(TraceTargetName, TraceEffectEnd);
	}
}

void USWeaponEffects::PlayMuzzleEffect() const
{
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponActor->SkeletalMeshComp, MuzzleSocketName);
}

void USWeaponEffects::PlayEffectsOnImpact(const FHitResult& HitResult) const
{
	const EPhysicalSurface HitSurfaceType = UGameplayStatics::GetSurfaceType(HitResult);

	UParticleSystem* ImpactEffect = nullptr;

	switch (HitSurfaceType)
	{
	case SURFACE_FLESH_DEFAULT:
		ImpactEffect = FleshImpactEffect;
		break;
	case SURFACE_FLESH_VULNERABLE:
		ImpactEffect = VulnerableFleshImpactEffect;
		break;
	default:
		ImpactEffect = DefaultImpactEffect;
		break;
	}

	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
            this,
            ImpactEffect,
            HitResult.ImpactPoint,
            HitResult.ImpactNormal.Rotation());
	}
}




