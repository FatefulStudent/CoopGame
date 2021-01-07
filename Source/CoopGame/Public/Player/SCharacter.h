#pragma once

#include "GameFramework/Character.h"

#include "SCharacter.generated.h"

class USHealthComponent;
class ASWeapon;
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
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Health)
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category=Zoom, meta = (ClampMin = 0.01, ClampMax = 180.0f))
	float ZoomedFOV = 65.0f;

	UPROPERTY(EditDefaultsOnly, Category=Zoom, meta = (ClampMin = 0.01, ClampMax = 100.0f))
	float ZoomInterpSpeed = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	TSubclassOf<ASWeapon> InitialWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	FName WeaponSocketName = TEXT("WeaponSocket");

	UPROPERTY(BlueprintReadOnly, Category=Health)
	bool bDied = false;
	
	// whether we want to zoom or not
	bool bWantsToZoom = false;
	
	// FOV cached from camera at BeginPlay
	float DefaultFOV = 90.0f;

	UPROPERTY()
	ASWeapon* CurrentWeapon;

public:
	ASCharacter();

	/** Returns	Camera location */
	virtual FVector GetPawnViewLocation() const override;

protected:
	// ACharacter overrides
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~ACharacter overrides
	
	void SpawnWeapon();
	void TickCamera(float DeltaSeconds);

	void StartFiring();
	void StopFiring();
	void Reload();
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();
	
	void BeginZoom();
	void EndZoom();

	UFUNCTION()
    void OnHealthChanged(USHealthComponent* _, int32 HealthDelta);
	void KillCharacter();
	
};
