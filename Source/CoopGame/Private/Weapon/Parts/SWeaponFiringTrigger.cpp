#include "Weapon/Parts/SWeaponFiringTrigger.h"
#include "Weapon/Parts/SWeaponClip.h"
#include "Weapon/Parts/SWeaponShooter.h"
#include "Weapon/SWeapon.h"
#include "Helpers/NetworkHelper.h"

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
	check (FNetworkHelper::HasAuthority(this) || FNetworkHelper::IsLocallyControlled(this));
		
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
	check (FNetworkHelper::HasAuthority(this) || FNetworkHelper::IsLocallyControlled(this));
	
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
	check (FNetworkHelper::HasAuthority(this) || FNetworkHelper::IsLocallyControlled(this));
	
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

	if (Shooter &&
		(FNetworkHelper::HasAuthority(this) || FNetworkHelper::IsLocallyControlled(this)))
	{
		Shooter->OnFired.AddUObject(this, &USWeaponFiringTrigger::OnWeaponFired);
	}
}

void USWeaponFiringTrigger::OnWeaponFired()
{
	LastFireTime = WeaponActor->GetWorld()->GetTimeSeconds();
}
