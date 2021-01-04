#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS(meta=(ChildCannotTick))
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Camera)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Camera)
	USpringArmComponent* SpringArmComp;

public:
	ASCharacter();

	/** Returns	Camera location */
	virtual FVector GetPawnViewLocation() const override;

protected:
	// ACharacter overrides
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// ~ACharacter overrides

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();
};
