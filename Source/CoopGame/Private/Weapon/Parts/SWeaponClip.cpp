#include "Weapon/Parts/SWeaponClip.h"


USWeaponClip::USWeaponClip()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool USWeaponClip::HasBullets() const
{
	return BulletsInClip > 0;
}

void USWeaponClip::SpendBullet()
{
	if (!ensureAlways(HasBullets()))
		return;

	BulletsInClip--;
}

void USWeaponClip::Reload()
{
	BulletsInClip = ClipCapacity;
}

void USWeaponClip::BeginPlay()
{
	Super::BeginPlay();

	BulletsInClip = ClipCapacity;
}