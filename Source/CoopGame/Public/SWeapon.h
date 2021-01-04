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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effects)
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effects)
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effects)
	FName MuzzleSocketName = TEXT("MuzzleSocket");

public:	
	ASWeapon();

protected:
	UFUNCTION(BlueprintCallable, Category=Firing)
	void Fire();

private:
	void PlayEffectsOnShoot() const;
	void PlayEffectsOnImpact(const FVector& ImpactLocation, const FRotator& ImpactRotation) const;
	void ApplyDamageToHitActor(const FVector& ShotDirection, const FHitResult& HitResult);
	void Shoot(
		const FVector& TraceStart,
		const FVector& TraceEnd,
		const FVector& ShotDirection);
};
