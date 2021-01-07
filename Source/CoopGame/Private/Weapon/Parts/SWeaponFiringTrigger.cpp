#include "Weapon/Parts/SWeaponFiringTrigger.h"
#include "Weapon/Parts/SWeaponClip.h"
#include "Weapon/Parts/SWeaponShooter.h"
#include "Weapon/SWeapon.h"

#define SECONDS_IN_MINUTE 60.0f

USWeaponFiringTrigger::USWeaponFiringTrigger()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USWeaponFiringTrigger::InitConstructor(USWeaponShooter* InShooter, USWeaponClip* InClip)
{
	Shooter = InShooter;
	Clip = InClip;
}

void USWeaponFiringTrigger::StartFiring()
{
	const float TimePassedSinceLastFire = GetWorld()->GetTimeSeconds() - LastFireTime;
	const float FirstDelay = FMath::Max(0.0f, TimeBetweenShots - TimePassedSinceLastFire);

	if (IsValid(WeaponActor))
	{
		WeaponActor->GetWorldTimerManager().SetTimer(
	        TimerHandle_TimeBetweenShots,
	        this,
	        &USWeaponFiringTrigger::AttemptToFire,
	        TimeBetweenShots,
	        true,
	        FirstDelay);
	}
}

void USWeaponFiringTrigger::AttemptToFire()
{
	if (Clip->HasBullets())
	{
		Shooter->Fire();
	}
	else
	{
		StopFiring();
	}
	
}

void USWeaponFiringTrigger::StopFiring()
{
	if (IsValid(WeaponActor))
	{
		WeaponActor->GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	}
}

void USWeaponFiringTrigger::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = SECONDS_IN_MINUTE / RateOfFire;
	WeaponActor = Cast<ASWeapon>(GetOwner());

	if (Shooter)
	{
		Shooter->OnFired.AddUObject(this, &USWeaponFiringTrigger::OnWeaponFired);
	}
}

void USWeaponFiringTrigger::OnWeaponFired()
{
	LastFireTime = WeaponActor->GetWorld()->GetTimeSeconds();
}