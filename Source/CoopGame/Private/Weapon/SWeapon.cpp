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
	FiringTrigger->StartFiring();
}

void ASWeapon::StopFiring()
{
	FiringTrigger->StopFiring();
}

void ASWeapon::Reload()
{
	Clip->Reload();
}
