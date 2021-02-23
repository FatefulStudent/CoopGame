#pragma once

#include "Interfaces/Interactable.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SPickUp.generated.h"

class USphereComponent;
class ASPowerUp;

UCLASS()
class COOPGAME_API ASPickUp : public AActor, public IInteractable
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, Category=Collision)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category=Visual)
	UDecalComponent* DecalComp;

	UPROPERTY(EditDefaultsOnly, Category=Visual)
	TSubclassOf<ASPowerUp> PowerUpClass;

	UPROPERTY(EditDefaultsOnly, Category=Visual)
	float RespawnInterval = 5.0f;

private:
	UPROPERTY() ASPowerUp* SpawnedPowerUp;
	bool OnCooldown = false;

	FTimerHandle RespawnPowerUp_TimerHandle;
	
public:
	ASPickUp();

	// IInteractive interface
	virtual bool IsAvailableForInteraction() const override;
	virtual void OnSuccessfulInteraction() override;
	// ~IInteractive interface

protected:
	virtual void BeginPlay() override;

	void RespawnPowerUp();
};
