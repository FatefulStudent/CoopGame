#pragma once

#include "Interfaces/Interactable.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SPickUp.generated.h"

class USphereComponent;

UCLASS()
class COOPGAME_API ASPickUp : public AActor, public IInteractable
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, Category=Collision)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category=Visual)
	UDecalComponent* DecalComp;

private:
	bool OnCooldown = false;
	
public:
	ASPickUp();

	// IInteractive interface
	virtual bool IsAvailableForInteraction() const override;
	virtual void OnSuccessfulInteraction() override;
	// ~IInteractive interface

protected:
	virtual void BeginPlay() override;

};
