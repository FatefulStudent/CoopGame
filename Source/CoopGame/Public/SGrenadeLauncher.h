#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SProjectile.h"

#include "SGrenadeLauncher.generated.h"

UCLASS()
class COOPGAME_API ASGrenadeLauncher : public ASWeapon
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=ProjectileParams)
	TSubclassOf<ASProjectile> ProjectileClass;

	virtual void Shoot(const FVector& TraceStart, const FVector& TraceEnd, const FVector& ShotDirection) override;
};
