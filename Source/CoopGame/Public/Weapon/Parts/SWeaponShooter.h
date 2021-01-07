#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SWeaponShooter.generated.h"

class USWeaponClip;
class USWeaponEffects;
class ASWeapon;

UCLASS( ClassGroup=(WeaponParts) )
class COOPGAME_API USWeaponShooter : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE(FOnFiredEventSignature)
    FOnFiredEventSignature OnFired;
	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	TSubclassOf<UDamageType> DamageType = UDamageType::StaticClass();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	float BaseDamage = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	float VulnerableFleshDamageMultiplier = 2.0f;

private:
	UPROPERTY()
	USWeaponClip* Clip;

	UPROPERTY()
	USWeaponEffects* WeaponEffects;
	
	UPROPERTY()
	ASWeapon* WeaponActor;
	
	UPROPERTY()
	APawn* PawnThatOwnsWeaponActor; 
	
public:
	USWeaponShooter();
	void InitConstructor(USWeaponClip* InClip, USWeaponEffects* InWeaponEffects);
	void Fire();

protected:
	virtual void BeginPlay() override;

private:
	void Shoot(
        const FVector& TraceStart,
        const FVector& TraceEnd,
        const FVector& ShotDirection);

	void DrawDebug(const FVector& TraceStart, const FVector& TraceEnd) const;

	void ApplyPointDamageToHitActor(const FVector& ShotDirection, const FHitResult& HitResult);
};
