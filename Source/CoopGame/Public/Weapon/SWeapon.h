#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SWeapon.generated.h"

class USWeaponEffects;
class USWeaponShooter;
class USWeaponFiringTrigger;
class USWeaponClip;
class USkeletalMeshComponent;
class UDamageType;

UCLASS(meta=(ChildCannotTick))
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Visual)
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Visual)
	FName MuzzleSocketName = TEXT("MuzzleSocket");
	
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Parts)
	USWeaponClip* Clip;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Parts)
	USWeaponEffects* Effects;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Parts)
	USWeaponShooter* Shooter;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Parts)
	USWeaponFiringTrigger* FiringTrigger;
	
public:
	ASWeapon();

	// StartFiring
	void StartFiring();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFiring();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastStartFiring();
	// ~StartFiring

	// StartFiring
	void StopFiring();

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerStopFiring();
		
	UFUNCTION(NetMulticast, Reliable)
    void NetMulticastStopFiring();
	// ~StartFiring

	// Reload
	void Reload();
	
	UFUNCTION(Server, Reliable, WithValidation)
    void ServerReload();
			
	UFUNCTION(NetMulticast, Reliable)
    void NetMulticastReload();

	// ~Reload
	
};
