#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerUp.generated.h"

UCLASS()
class COOPGAME_API ASPowerUp : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	int32 TotalNumberOfTicks = 0;

	UPROPERTY(EditDefaultsOnly)
	float TickInterval = 0.0f;

	FTimerHandle Tick_TimerHandle;

	int32 NumberOfTicksProcessed = 0;
	
public:	
	UFUNCTION(BlueprintImplementableEvent, Category=PowerUps)
	void OnPowerUpActivated();
	
	UFUNCTION(BlueprintImplementableEvent, Category=PowerUps)
	void OnPowerUpTicked();
	
	UFUNCTION(BlueprintImplementableEvent, Category=PowerUps)
	void OnPowerUpExpired();

	void ActivatePowerUp();
	
protected:
	void OnTicked();

};
