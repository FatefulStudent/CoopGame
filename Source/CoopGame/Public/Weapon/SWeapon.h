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
	
	void StartFiring();
	
	void StopFiring();

	void Reload();
	
protected:
	UFUNCTION(Server, Reliable, WithValidation)
    void ServerStartFiring();
	
	UFUNCTION(Server, Reliable, WithValidation)
    void ServerStopFiring();

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerReload();
			
	UFUNCTION(Client, Reliable)
    void ClientReload();
};
