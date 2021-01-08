#include "Weapon/Parts/SWeaponEffects.h"
#include "Weapon/SWeapon.h"
#include "CoopGame/CoopGame.h"
#include "Helpers/NetworkHelper.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

USWeaponEffects::USWeaponEffects()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USWeaponEffects::BeginPlay()
{
	Super::BeginPlay();

	WeaponActor = Cast<ASWeapon>(GetOwner());
	
	if (!WeaponActor)
		ensureAlways(false);
}

void USWeaponEffects::PlayCameraShake() const
{
	check(FNetworkHelper::HasCosmetics(this));

	if (WeaponActor)
	{
		if (APlayerController* InstigatorPlayerController = Cast<APlayerController>(WeaponActor->GetInstigatorController()))
		{
			InstigatorPlayerController->ClientPlayCameraShake(CameraShakeClass);
		}
	}
}

void USWeaponEffects::PlayMuzzleEffect() const
{
	check(FNetworkHelper::HasCosmetics(this));
	
	if (MuzzleEffect && WeaponActor)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponActor->SkeletalMeshComp, WeaponActor->MuzzleSocketName);
}

void USWeaponEffects::PlayTraceEffect(const FVector& TraceEffectEnd) const
{
	check(FNetworkHelper::HasCosmetics(this));
	
	if (TracerEffect && WeaponActor)
	{
		const FVector MuzzleLocation = WeaponActor->SkeletalMeshComp->GetSocketLocation(WeaponActor->MuzzleSocketName);

		UParticleSystemComponent* TraceParticlesComp =
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		TraceParticlesComp->SetVectorParameter(TraceTargetName, TraceEffectEnd);
	}
}

void USWeaponEffects::PlayEffectsOnImpact(const FVector& ImpactPoint, const EPhysicalSurface HitSurfaceType,
	const FVector& ShotDirection) const
{
	check(FNetworkHelper::HasCosmetics(this));

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
            ImpactPoint,
            ShotDirection.Rotation());
	}
}




