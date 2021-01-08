#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SWeaponFiringTrigger.generated.h"


class USWeaponClip;
class USWeaponShooter;
class ASWeapon;

UCLASS( ClassGroup=(WeaponParts), meta=(ChildCannotTick) )
class COOPGAME_API USWeaponFiringTrigger : public UActorComponent
{
	GENERATED_BODY()

protected:
	// RPM - bullets per minute fired  
	UPROPERTY(EditDefaultsOnly, Category=Firing, meta=(ClampMin=1))
	float RateOfFire = 600.0f;

private:
	UPROPERTY()
	USWeaponShooter* Shooter;
	
	UPROPERTY()
	USWeaponClip* Clip;
	
	UPROPERTY()
	ASWeapon* WeaponActor;

	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastFireTime = -1.0f;

	// Sets on BeginPlay, derives from RateOfFire
	float TimeBetweenShots;
	
public:	
	USWeaponFiringTrigger();
	void InitConstructor(USWeaponShooter* InShooter, USWeaponClip* InClip);

	void StartFiring();
	void AttemptToFire();
	void StopFiring();
	
protected:
	virtual void BeginPlay() override;

private:
	void OnWeaponFired();

};
