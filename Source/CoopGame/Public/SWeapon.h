#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;

UCLASS(meta=(ChildCannotTick))
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Visual)
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	TSubclassOf<UDamageType> DamageType = UDamageType::StaticClass();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	float BaseDamage = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	float VulnerableFleshDamageMultiplier = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|CameraShake")
	TSubclassOf<UCameraShake> CameraShakeClass = UCameraShake::StaticClass();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|MuzzleFlash")
	FName MuzzleSocketName = TEXT("MuzzleSocket");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|MuzzleFlash")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Impact")
	UParticleSystem* DefaultImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Impact")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Impact")
	UParticleSystem* VulnerableFleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Trace")
	UParticleSystem* TracerEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects|Trace")
	FName TraceTargetName = TEXT("Target");

public:	
	ASWeapon();

	UFUNCTION(BlueprintCallable, Category=Firing)
	void Fire();

protected:

	virtual void Shoot(
		const FVector& TraceStart,
		const FVector& TraceEnd,
		const FVector& ShotDirection);

	void PlayCameraShake() const;
	void PlayFireEffects(const FVector& TraceEffectEnd) const;
	void PlayTraceEffect(const FVector& TraceEffectEnd) const;
	void PlayMuzzleEffect() const;
	void PlayEffectsOnImpact(const FHitResult& HitResult) const;

	void DrawDebug(const FVector& TraceStart, const FVector& TraceEnd) const;

private:
	void ApplyPointDamageToHitActor(const FVector& ShotDirection, const FHitResult& HitResult);
};
