#include "LevelObjects/SPickUp.h"

#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Helpers/NetworkHelper.h"
#include "LevelObjects/SPowerUp.h"


ASPickUp::ASPickUp()
{
	SetReplicates(true);
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetSphereRadius(75.0f);
	RootComponent = CollisionComp;
	
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComp->SetupAttachment(RootComponent);

	PowerUpClass = ASPowerUp::StaticClass();
}

bool ASPickUp::IsAvailableForInteraction() const
{
	return !OnCooldown;
}

void ASPickUp::OnSuccessfulInteraction(APawn* InteractionInstigator)
{
	check(FNetworkHelper::HasAuthority(this));
	
	check(InteractionInstigator);
	
	UE_LOG(LogTemp, Warning, TEXT("Pickup %s was picked up by "), *GetName(), *InteractionInstigator->GetName())
	OnCooldown = true;
	SpawnedPowerUp->ActivatePowerUp(InteractionInstigator);
	SpawnedPowerUp->SetActorEnableCollision(false);
	SpawnedPowerUp->SetActorHiddenInGame(true);

	GetWorldTimerManager().SetTimer(RespawnPowerUp_TimerHandle,
		this, &ASPickUp::RespawnPowerUp, RespawnInterval);
}

void ASPickUp::BeginPlay()
{
	Super::BeginPlay();

	if (FNetworkHelper::HasAuthority(this))
		RespawnPowerUp();
}

void ASPickUp::RespawnPowerUp()
{
	check(FNetworkHelper::HasAuthority(this));
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	SpawnedPowerUp = GetWorld()->SpawnActor<ASPowerUp>(PowerUpClass, GetTransform(), SpawnParameters);
	GetWorldTimerManager().ClearTimer(RespawnPowerUp_TimerHandle);

	OnCooldown = false;
}
