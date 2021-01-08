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
	
	if (APlayerController* InstigatorPlayerController = Cast<APlayerController>(WeaponActor->GetInstigatorController()))
	{
		InstigatorPlayerController->ClientPlayCameraShake(CameraShakeClass);
	}
}

void USWeaponEffects::PlayMuzzleEffect() const
{
	check(FNetworkHelper::HasCosmetics(this));
	
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponActor->SkeletalMeshComp, WeaponActor->MuzzleSocketName);
}

void USWeaponEffects::PlayTraceEffect(const FVector& TraceEffectEnd) const
{
	check(FNetworkHelper::HasCosmetics(this));
	
	if (TracerEffect)
	{
		const FVector MuzzleLocation = WeaponActor->SkeletalMeshComp->GetSocketLocation(WeaponActor->MuzzleSocketName);

		UParticleSystemComponent* TraceParticlesComp =
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		TraceParticlesComp->SetVectorParameter(TraceTargetName, TraceEffectEnd);
	}
}

void USWeaponEffects::PlayEffectsOnImpact(const FHitResult& HitResult) const
{
	check(FNetworkHelper::HasCosmetics(this));
	
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




