#include "Weapon/SWeapon.h"
#include "Weapon/Parts/SWeaponClip.h"
#include "Weapon/Parts/SWeaponFiringTrigger.h"
#include "Weapon/Parts/SWeaponShooter.h"
#include "Weapon/Parts/SWeaponEffects.h"
#include "Helpers/NetworkHelper.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ASWeapon::ASWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
	NetUpdateFrequency = 64.0f;
	MinNetUpdateFrequency = 32.0f;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SkeletalMeshComp;
	
	Clip = CreateDefaultSubobject<USWeaponClip>(TEXT("Clip"));
	Effects = CreateDefaultSubobject<USWeaponEffects>(TEXT("Effects"));
	
	Shooter = CreateDefaultSubobject<USWeaponShooter>(TEXT("Shooter"));
	Shooter->InitConstructor(Clip, Effects);

	FiringTrigger = CreateDefaultSubobject<USWeaponFiringTrigger>(TEXT("FiringTrigger"));
	FiringTrigger->InitConstructor(Shooter, Clip);
}

void ASWeapon::StartFiring()
{
	check(FNetworkHelper::IsLocallyControlled(this));

	ServerStartFiring();

	// if we are autonomous client we want to simulate firing on our side
	if (FNetworkHelper::IsAutonomousProxy(this))
		FiringTrigger->StartFiring();
}

void ASWeapon::ServerStartFiring_Implementation()
{
	FiringTrigger->StartFiring();
}

bool ASWeapon::ServerStartFiring_Validate()
{
	return true;
}

void ASWeapon::StopFiring()
{
	check(FNetworkHelper::IsLocallyControlled(this));

	ServerStopFiring();

	// if we are autonomous client we want to simulate firing on our side
	if (FNetworkHelper::IsAutonomousProxy(this))
		FiringTrigger->StopFiring();
}

void ASWeapon::ServerStopFiring_Implementation()
{
	FiringTrigger->StopFiring();
}

bool ASWeapon::ServerStopFiring_Validate()
{
	return true;
}

void ASWeapon::Reload()
{
	if (FNetworkHelper::HasAuthority(this) || FNetworkHelper::IsLocallyControlled(this))
		ServerReload();
}

void ASWeapon::ServerReload_Implementation()
{
	Clip->Reload();

	if (!FNetworkHelper::IsLocallyControlled(this))
		ClientReload();	
}

bool ASWeapon::ServerReload_Validate()
{
	return true;
}

void ASWeapon::ClientReload_Implementation()
{
	Clip->Reload();
}

