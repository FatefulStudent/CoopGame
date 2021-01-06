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

	UPROPERTY(EditDefaultsOnly, Category=Zoom)
	float ZoomedFOV = 65.0f;

	UPROPERTY(EditDefaultsOnly, Category=Zoom, meta = (ClampMin = 0.01, ClampUI))
	float ZoomInterpSpeed = 50.0f;
	
	// whether we want to zoom or not
	bool bWantsToZoom = false;
	
	// FOV cached from camera at BeginPlay
	float DefaultFOV;

public:
	ASCharacter();

	/** Returns	Camera location */
	virtual FVector GetPawnViewLocation() const override;

protected:
	// ACharacter overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// ~ACharacter overrides

	void TickCamera(float DeltaSeconds);
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();
	
	void BeginZoom();
	void EndZoom();
	
};
