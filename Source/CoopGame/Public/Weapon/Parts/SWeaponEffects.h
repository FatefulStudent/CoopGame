#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/SWeapon.h"

#include "SWeaponEffects.generated.h"


UCLASS( ClassGroup=(WeaponParts) )
class COOPGAME_API USWeaponEffects : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|CameraShake")
	TSubclassOf<UCameraShake> CameraShakeClass = UCameraShake::StaticClass();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|MuzzleFlash")
	FName MuzzleSocketName = TEXT("MuzzleSocket");
	
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

	void PlayFireEffects(const FVector& TraceEffectEnd) const;
	void PlayCameraShake() const;
	void PlayTraceEffect(const FVector& TraceEffectEnd) const;
	void PlayMuzzleEffect() const;
	void PlayEffectsOnImpact(const FHitResult& HitResult) const;

protected:
	virtual void BeginPlay() override;
};
