#include "Weapon/Parts/SWeaponShooter.h"
#include "Weapon/Parts/SWeaponClip.h"
#include "Weapon/Parts/SWeaponEffects.h"
#include "Weapon/Projectiles/SGrenadeProjectile.h"
#include "Weapon/SWeapon.h"
#include "CoopGame/CoopGame.h"
#include "Helpers/NetworkHelper.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
    TEXT("COOP.DebugWeapons"),
    DebugWeaponDrawing,
    TEXT("Draw debug lines when firing hitscan weapon"),
    ECVF_Cheat
);

namespace WeaponShooterLocal
{
	void GetHitScanTraceParams(FVector& ShotDirection, FVector& TraceStart, FVector& TraceEnd, APawn* PawnOwner)
	{
		FRotator EyeRotation;
		PawnOwner->GetActorEyesViewPoint(TraceStart, EyeRotation);

		ShotDirection = EyeRotation.Vector();
		TraceEnd = TraceStart + ShotDirection * 10000.0f;
	}
}

USWeaponShooter::USWeaponShooter()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USWeaponShooter::InitConstructor(USWeaponClip* InClip, USWeaponEffects* InWeaponEffects)
{
	Clip = InClip;
	WeaponEffects = InWeaponEffects;
}

void USWeaponShooter::Fire()
{
	if (!ensureAlways(Clip->HasBullets()))
		return;

	if (FNetworkHelper::HasCosmetics(this))
	{
		WeaponEffects->PlayCameraShake();
		WeaponEffects->PlayMuzzleEffect();
	}
	
	if (bShootProjectiles)
		ShootProjectile();
	else
		ShootHitScan();

	Clip->SpendBullet();
	OnFired.Broadcast();
}

void USWeaponShooter::BeginPlay()
{
	Super::BeginPlay();

	WeaponActor = Cast<ASWeapon>(GetOwner());
	
	if (!WeaponActor)
		ensureAlways(false);
}

void USWeaponShooter::ShootProjectile()
{
	APawn* PawnActor = Cast<APawn>(WeaponActor->GetOwner());
	// try and fire a projectile
	if (ProjectileClass && PawnActor && FNetworkHelper::HasAuthority(this))
	{
		SpawnProjectileAtMuzzle(PawnActor);
	}
}

void USWeaponShooter::SpawnProjectileAtMuzzle(APawn* PawnActor) const
{
	check(FNetworkHelper::HasAuthority(this));
	
	// Grabs location from the mesh that must have a socket called "Muzzle" in his skeleton
	const FVector MuzzleLocation = WeaponActor->SkeletalMeshComp->GetSocketLocation(WeaponActor->MuzzleSocketName);
	// Use controller rotation which is our view direction in first person
	const FRotator& MuzzleRotation = PawnActor->GetControlRotation();

	//Set Spawn Collision Handling Override
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = PawnActor;
	SpawnParameters.Instigator = PawnActor;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// spawn the projectile at the muzzle
	GetWorld()->SpawnActor<ASGrenadeProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParameters);
}

void USWeaponShooter::ShootHitScan()
{
	FVector TraceEffectEnd;
	PerformHitScanShot(TraceEffectEnd);

	if (FNetworkHelper::HasCosmetics(this))
		WeaponEffects->PlayTraceEffect(TraceEffectEnd);
}

void USWeaponShooter::PerformHitScanShot(FVector& TraceEffectEnd)
{	
	// Trace the world from the owners eyes perspective
	APawn* InstigatorActor = WeaponActor->GetInstigator();
	if (!InstigatorActor)
		return;

	FVector ShotDirection, TraceStart, TraceEnd;
	WeaponShooterLocal::GetHitScanTraceParams(ShotDirection, TraceStart, TraceEnd, InstigatorActor);

	FHitResult HitResult;
	const bool bBlockingHit = PerformLineTrace(TraceStart, TraceEnd, HitResult);

	if (bBlockingHit)
	{
		if (FNetworkHelper::HasAuthority(this))
			ApplyPointDamageToHitActor(ShotDirection, HitResult);

		TraceEffectEnd = HitResult.ImpactPoint;
		if (FNetworkHelper::HasCosmetics(this))
			WeaponEffects->PlayEffectsOnImpact(HitResult);
	}
	else
		TraceEffectEnd = TraceEnd;

	if (DebugWeaponDrawing > 0 && FNetworkHelper::HasCosmetics(this))
		DrawDebug(TraceStart, TraceEnd);
}

bool USWeaponShooter::PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const
{
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(WeaponActor);
	CollisionQueryParams.AddIgnoredActor(WeaponActor->GetInstigator());
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bReturnPhysicalMaterial = true;

	return GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_WeaponTraceChannel,
        CollisionQueryParams);
}

void USWeaponShooter::ApplyPointDamageToHitActor(const FVector& ShotDirection, const FHitResult& HitResult)
{
	check(FNetworkHelper::HasAuthority(this));
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

void USWeaponShooter::DrawDebug(const FVector& TraceStart, const FVector& TraceEnd) const
{
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f);
}
