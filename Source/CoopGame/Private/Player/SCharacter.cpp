#include "Player/SCharacter.h"
#include "Helpers/NetworkHelper.h"
#include "Common/Components/SHealthComponent.h"
#include "Weapon/SWeapon.h"
#include "CoopGame/CoopGame.h"
#include "Interfaces/Interactable.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "LevelObjects/SPickUp.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogSCharacter, Log, All)

ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	ACharacter::SetReplicateMovement(true);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	ACharacter::GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health"));
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WeaponTraceChannel, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	if (FNetworkHelper::HasAuthority(this))
	{
		GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	}
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	

	if (FNetworkHelper::HasAuthority(this))
	{
		SpawnWeapon();
	}
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

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::Reload);
}

void ASCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (ensureAlways(HasAuthority()))
		TryInteractingWith(OtherActor);
}

void ASCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (FNetworkHelper::HasAuthority(this))
	{
		if (CurrentWeapon)
			CurrentWeapon->Destroy();
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, bDied);
	DOREPLIFETIME(ASCharacter, CurrentWeapon);
}

bool ASCharacter::WantToInteract(IInteractable* Interactive) const
{
	return true;
}

void ASCharacter::Interact(IInteractable* Interactive)
{
	check(HasAuthority());
	
	if (!ensureAlways(WantToInteract(Interactive)))
		return;
	
	if (Interactive->IsAvailableForInteraction())
	{
		if (Cast<ASPickUp>(Interactive))
		{
			Interactive->OnSuccessfulInteraction(this);
		}
		else
		{
			UE_LOG(LogSCharacter,
                Log,
                TEXT("%s: Tried to interact with %s, but it was an unknown pickup."),
                *GetName(),
                *Cast<UObject>(Interactive)->GetName())
		}
	}
	else
	{
		UE_LOG(LogSCharacter,
            Log,
            TEXT("%s: Tried to interact with %s, but it was not available for interaction."),
            *GetName(),
            *Cast<UObject>(Interactive)->GetName())
	}
	
}

void ASCharacter::TryInteractingWith(AActor* OtherActor)
{
	if (IInteractable* OverlappedInteractive = Cast<IInteractable>(OtherActor))
	{
		if (WantToInteract(OverlappedInteractive))
		{
			Interact(OverlappedInteractive);
		}
		else
		{
			UE_LOG(LogSCharacter,
                Log,
                TEXT("%s: Tried to interact with %s, but character didn't want to interact with it."),
                *GetName(),
                *OtherActor->GetName())
		}
	}
	else
	{
		const FString ActorName = OtherActor ? OtherActor->GetName() : "NULL Actor Name";	
		UE_LOG(LogSCharacter,
            Log,
            TEXT("%s: Overlapped with %s, but it was not an interactable, or was NULL."),
            *GetName(), *ActorName);
	}
}

void ASCharacter::SpawnWeapon()
{
	check(FNetworkHelper::HasAuthority(this));
	
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

void ASCharacter::Reload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
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

void ASCharacter::OnHealthChanged(USHealthComponent* _, int32 HealthDelta)
{
	if (HealthComp->GetCurrentHealthPoints() <= 0 && !bDied)
	{
		// Die!
		if (FNetworkHelper::HasAuthority(this))
			KillCharacter();
	}
}

void ASCharacter::KillCharacter()
{
	check(FNetworkHelper::HasAuthority(this));
	
	if (!ensureAlways(!bDied))
		return;

	bDied = true;
	OnRep_bDied();

	DetachFromControllerPendingDestroy();
	SetLifeSpan(10.0f);
}

void ASCharacter::OnRep_bDied()
{
	if (bDied)
	{
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		GetCapsuleComponent()->SetEnableGravity(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetEnableGravity(false);
	}
}
