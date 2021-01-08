#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SWeaponClip.generated.h"


UCLASS( ClassGroup=(WeaponParts), meta=(ChildCannotTick) )
class COOPGAME_API USWeaponClip : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=Capacity, meta=(ClampMin=1))
	int32 ClipCapacity = 30;

private:
	int32 BulletsInClip = ClipCapacity;
	
public:
	USWeaponClip();

	bool HasBullets() const;
	void SpendBullet();
	void Reload();
	
protected:
	virtual void BeginPlay() override;
};
