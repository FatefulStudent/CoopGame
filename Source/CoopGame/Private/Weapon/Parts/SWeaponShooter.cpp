#include "Weapon/Parts/SWeaponShooter.h"
#include "Weapon/Parts/SWeaponClip.h"
#include "Weapon/Parts/SWeaponEffects.h"
#include "Weapon/SWeapon.h"
#include "CoopGame/CoopGame.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
    TEXT("COOP.DebugWeapons"),
    DebugWeaponDrawing,
    TEXT("Draw debug lines when firing hitscan weapon"),
    ECVF_Cheat
);

USWeaponShooter::USWeaponShooter()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USWeaponShooter::InitConstructor(USWeaponClip* InClip, USWeaponEffects* InWeaponEffects)
{
	Clip = InClip;
	WeaponEffects = InWeaponEffects;
}

void USWeaponShooter::Fire()
{	
	// Trace the world from the owners eyes perspective
	if (AActor* InstigatorActor = WeaponActor->GetInstigator())
	{
		FVector TraceStart;
		FRotator EyeRotation;
		InstigatorActor->GetActorEyesViewPoint(TraceStart, EyeRotation);

		const FVector ShotDirection = EyeRotation.Vector();
		const FVector TraceEnd = TraceStart + ShotDirection * 10000.0f;

		Shoot(TraceStart, TraceEnd, ShotDirection);
		Clip->SpendBullet();
		OnFired.Broadcast();
	}
}

void USWeaponShooter::BeginPlay()
{
	Super::BeginPlay();

	WeaponActor = Cast<ASWeapon>(GetOwner());
	
	if (!WeaponActor)
		ensureAlways(false);
}

void USWeaponShooter::Shoot(const FVector& TraceStart, const FVector& TraceEnd, const FVector& ShotDirection)
{
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(WeaponActor);
	CollisionQueryParams.AddIgnoredActor(WeaponActor->GetInstigator());
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	FHitResult HitResult;
	const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        COLLISION_WEAPON,
        CollisionQueryParams);

	if (bBlockingHit)
	{
		if (WeaponActor->HasAuthority())
			ApplyPointDamageToHitActor(ShotDirection, HitResult);

		WeaponEffects->PlayEffectsOnImpact(HitResult);
	}

	const FVector& TraceEffectEnd = bBlockingHit ? HitResult.ImpactPoint : TraceEnd;

	WeaponEffects->PlayFireEffects(TraceEffectEnd);

	if (DebugWeaponDrawing > 0)
		DrawDebug(TraceStart, TraceEnd);
}

void USWeaponShooter::DrawDebug(const FVector& TraceStart, const FVector& TraceEnd) const
{
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f);
}

void USWeaponShooter::ApplyPointDamageToHitActor(const FVector& ShotDirection, const FHitResult& HitResult)
{
	float DamageToApply = BaseDamage;

	if (UGameplayStatics::GetSurfaceType(HitResult) == SURFACE_FLESH_VULNERABLE)
		DamageToApply *= VulnerableFleshDamageMultiplier;

	UGameplayStatics::ApplyPointDamage(
        HitResult.GetActor(),
        DamageToApply,
        ShotDirection,
        HitResult,
        WeaponActor->GetInstigatorController(),
        WeaponActor,
        DamageType);
}


