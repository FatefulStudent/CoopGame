#include "SCharacter.h"
#include "SWeapon.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	ACharacter::GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

}

FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	SpawnWeapon();
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	CurrentWeapon->SetInstigator(this);
}

void ASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickCamera(DeltaSeconds);
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::StopJumping);
	
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFiring);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFiring);
}

void ASCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentWeapon)
		CurrentWeapon->MarkPendingKill();
}

void ASCharacter::SpawnWeapon()
{
	FActorSpawnParameters WeaponSpawnParams;
	WeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	WeaponSpawnParams.Instigator = this;
	WeaponSpawnParams.Owner = this;

	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(InitialWeaponClass, WeaponSpawnParams);

	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(
	        GetMesh(),
	        FAttachmentTransformRules::SnapToTargetIncludingScale,
	        WeaponSocketName);
	}
}

void ASCharacter::TickCamera(float DeltaSeconds)
{
	const float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	const float CurrentFOV = CameraComp->FieldOfView;
	const float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}

void ASCharacter::StartFiring()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFiring();
	}
}

void ASCharacter::StopFiring()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}
