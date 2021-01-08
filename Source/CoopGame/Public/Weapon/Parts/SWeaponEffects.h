#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/SWeapon.h"

#include "SWeaponEffects.generated.h"


UCLASS( ClassGroup=(WeaponParts), meta=(ChildCannotTick) )
class COOPGAME_API USWeaponEffects : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|CameraShake")
	TSubclassOf<UCameraShake> CameraShakeClass = UCameraShake::StaticClass();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|MuzzleFlash")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Trace")
	UParticleSystem* TracerEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Trace")
	FName TraceTargetName = TEXT("Target");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Impact")
	UParticleSystem* DefaultImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Impact")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Impact")
	UParticleSystem* VulnerableFleshImpactEffect;

private:
	UPROPERTY()
	ASWeapon* WeaponActor;

public:
	USWeaponEffects();

	void PlayCameraShake() const;
	void PlayMuzzleEffect() const;
	void PlayTraceEffect(const FVector& TraceEffectEnd) const;
	void PlayEffectsOnImpact(const FVector& ImpactPoint, const EPhysicalSurface HitSurfaceType, const FVector& HitNormal) const;

protected:
	virtual void BeginPlay() override;
};
