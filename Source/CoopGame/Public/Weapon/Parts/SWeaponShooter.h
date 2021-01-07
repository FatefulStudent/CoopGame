#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SWeaponShooter.generated.h"

class ASGrenadeProjectile;
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

	UPROPERTY(EditDefaultsOnly, Category=ShootingMode)
	bool bShootProjectiles = false;
	
	UPROPERTY(EditDefaultsOnly, Category="ShootingMode|Projectile", meta = (EditCondition = "bShootProjectiles"))
	TSubclassOf<ASGrenadeProjectile> ProjectileClass;

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
	void ShootProjectile();
	void SpawnProjectileAtMuzzle(APawn* PawnActor) const;

	void ShootHitScan();
	void PerformHitScanShot(FVector& TraceEffectEnd);
	bool PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const;
	void ApplyPointDamageToHitActor(const FVector& ShotDirection, const FHitResult& HitResult);

	void DrawDebug(const FVector& TraceStart, const FVector& TraceEnd) const;
};
