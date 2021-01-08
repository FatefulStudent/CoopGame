#include "Weapon/SWeapon.h"

#include "Weapon/Parts/SWeaponClip.h"
#include "Weapon/Parts/SWeaponFiringTrigger.h"
#include "Weapon/Parts/SWeaponShooter.h"
#include "Weapon/Parts/SWeaponEffects.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ASWeapon::ASWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

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
	if (AActor* WeaponOwner = GetOwner())
	{
		if (WeaponOwner->GetLocalRole() >= ROLE_AutonomousProxy)
			ServerStartFiring();
	}
}

void ASWeapon::ServerStartFiring_Implementation()
{
	NetMulticastStartFiring();
}

bool ASWeapon::ServerStartFiring_Validate()
{
	return true;
}

void ASWeapon::NetMulticastStartFiring_Implementation()
{
	FiringTrigger->StartFiring();
}

void ASWeapon::StopFiring()
{
	if (AActor* WeaponOwner = GetOwner())
	{
		if (WeaponOwner->GetLocalRole() >= ROLE_AutonomousProxy)
			ServerStopFiring();
	}
}

void ASWeapon::ServerStopFiring_Implementation()
{
	NetMulticastStopFiring();
}

bool ASWeapon::ServerStopFiring_Validate()
{
	return true;
}

void ASWeapon::NetMulticastStopFiring_Implementation()
{
	FiringTrigger->StopFiring();
}

void ASWeapon::Reload()
{
	if (AActor* WeaponOwner = GetOwner())
	{
		if (WeaponOwner->GetLocalRole() >= ROLE_AutonomousProxy)
			ServerReload();
	}
}

void ASWeapon::ServerReload_Implementation()
{
	NetMulticastReload();	
}

bool ASWeapon::ServerReload_Validate()
{
	return true;
}

void ASWeapon::NetMulticastReload_Implementation()
{
	Clip->Reload();
}

