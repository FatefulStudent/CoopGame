#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;

UCLASS(meta=(ChildCannotTick))
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Visual)
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	TSubclassOf<UDamageType> DamageType = UDamageType::StaticClass();
	
public:	
	ASWeapon();
	

protected:
	UFUNCTION(BlueprintCallable, Category=Firing)
	void Fire();

private:
	void ShootAndDamageHitActor(
		AActor* OwnerActor,
		const FVector& TraceStart,
		const FVector& TraceEnd,
		const FVector& ShotDirection);
};
