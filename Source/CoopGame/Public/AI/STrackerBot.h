#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "STrackerBot.generated.h"

class USHealthComponent;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, Category=Visual)
	UStaticMeshComponent* StaticMeshComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category=Health)
	USHealthComponent* HealthComp;
	
	UPROPERTY(EditDefaultsOnly, Category=Health)
	FName LastTimeDamagedParameterName = TEXT("LastTimeDamaged");

	// How quickly it will move to next path point
	UPROPERTY(EditDefaultsOnly, Category=Movement)
	float RollingForceStrength = 100.0f;

	// When the target will be in acceptance radius movement will stop
	UPROPERTY(EditDefaultsOnly, Category=Movement)
	float AcceptanceRadiusToTarget = 50.0f;

	// If true, mass will have no effect on movement
	UPROPERTY(EditDefaultsOnly, Category=Movement)
	bool bAccelChange = true;

	// If true, mass will have no effect on movement
	UPROPERTY(BlueprintReadOnly, Category=Movement)
	// Next point in navigation path to player character
	FVector NextPathPoint = FVector(ForceInitToZero);

private:
	UPROPERTY()
	UMaterialInstanceDynamic* MaterialForPulseOnDamage;

public:
	ASTrackerBot();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void HandleHealthChanged(USHealthComponent* _, int32 HealthDelta);

	FVector CalculateNextPathPoint() const;

	void MoveToTargetByForce();
};
