#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, Category=Visual)
	UStaticMeshComponent* StaticMeshComp;

	// How quickly it will move to next path point
	UPROPERTY(EditDefaultsOnly, Category=Rolling)
	float RollingForceStrength = 100.0f;

	// When the target will be in acceptance radius movement will stop
	UPROPERTY(EditDefaultsOnly, Category=Rolling)
	float AcceptanceRadiusToTarget = 50.0f;

	// If true, mass will have no effect on movement
	UPROPERTY(EditDefaultsOnly, Category=Rolling)
	bool bAccelChange = true;

	// If true, mass will have no effect on movement
	UPROPERTY(BlueprintReadOnly, Category=Rolling)
	// Next point in navigation path to player character
	FVector NextPathPoint = FVector(ForceInitToZero);

public:
	ASTrackerBot();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FVector CalculateNextPathPoint() const;

	void MoveToTargetByForce();
};
