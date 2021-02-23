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

	TWeakObjectPtr<APawn> InteractionInstigator;
	
public:	
	UFUNCTION(BlueprintImplementableEvent, Category=PowerUps)
	void OnPowerUpActivated(APawn* InInteractionInstigator);
	
	UFUNCTION(BlueprintImplementableEvent, Category=PowerUps)
	void OnPowerUpTicked(APawn* InInteractionInstigator);
	
	UFUNCTION(BlueprintImplementableEvent, Category=PowerUps)
	void OnPowerUpExpired(APawn* InInteractionInstigator);


	ASPowerUp();
	
	void ActivatePowerUp(APawn* InInteractionInstigator);
	
protected:

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastPowerUpActivated(APawn* InInteractionInstigator);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastPowerUpTick();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastPowerUpExpired();

};
