#include "Weapon/Parts/SWeaponShooter.h"
#include "Weapon/Parts/SWeaponClip.h"
#include "Weapon/Parts/SWeaponEffects.h"
#include "Weapon/Projectiles/SGrenadeProjectile.h"
#include "Weapon/SWeapon.h"
#include "CoopGame/CoopGame.h"
#include "Helpers/NetworkHelper.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

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

void USWeaponShooter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(USWeaponShooter, ShootResult, COND_SkipOwner);
}

void USWeaponShooter::ShootProjectile()
{
	APawn* PawnActor = Cast<APawn>(WeaponActor->GetOwner());
	// try and fire a projectile
	if (ProjectileClass && PawnActor)
	{
		if (FNetworkHelper::HasAuthority(this))
			SpawnProjectileAtMuzzle(PawnActor);

		ShootResult = FShootResult
		{
			false
		};

		OnRep_ShootResult();
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
	check(FNetworkHelper::HasAuthority(this) || FNetworkHelper::IsLocallyControlled(this));

	ShootResult = FShootResult();
	
	// Trace the world from the owners eyes perspective
	APawn* InstigatorActor = WeaponActor->GetInstigator();
	if (!InstigatorActor)
		return;

	FVector ShotDirection, TraceStart, TraceEnd;
	WeaponShooterLocal::GetHitScanTraceParams(ShotDirection, TraceStart, TraceEnd, InstigatorActor);

	FHitResult HitResult;
	const bool bBlockingHit = PerformLineTrace(TraceStart, TraceEnd, HitResult);

	FVector TraceEffectEnd = TraceEnd;
	
	if (bBlockingHit)
	{
		if (FNetworkHelper::HasAuthority(this))
			ApplyPointDamageToHitActor(ShotDirection, HitResult);

		TraceEffectEnd = HitResult.ImpactPoint;
	}

	ShootResult = FShootResult
	{
		true,
	    bBlockingHit,
	    TraceEffectEnd,
	    UGameplayStatics::GetSurfaceType(HitResult)
	};

	OnRep_ShootResult();
}

bool USWeaponShooter::PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const
{
	check(FNetworkHelper::HasAuthority(this) || FNetworkHelper::IsLocallyControlled(this));
	
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

void USWeaponShooter::ApplyPointDamageToHitActor(const FVector& ShotDirection, const FHitResult& HitResult) const
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

void USWeaponShooter::OnRep_ShootResult() const
{
	if (FNetworkHelper::HasCosmetics(this))
	{
		PlayHitScanSpecificEffects();

		PlayLocallyControlledSpecificEffects();
		
		PlayCommonEffects();
	}
}

void USWeaponShooter::PlayHitScanSpecificEffects() const
{
	if (ShootResult.bHitScan)
	{
		WeaponEffects->PlayTraceEffect(ShootResult.TraceEnd);

		const FVector MuzzleLocation = WeaponActor->SkeletalMeshComp->GetSocketLocation(WeaponActor->MuzzleSocketName);
		const FVector ShotDirection = MuzzleLocation - ShootResult.TraceEnd;
		if (ShootResult.bBlockingHit)
			WeaponEffects->PlayEffectsOnImpact(ShootResult.TraceEnd, ShootResult.HitSurface, ShotDirection);
	}
}

void USWeaponShooter::PlayLocallyControlledSpecificEffects() const
{
	if (APawn* OwningPawn = WeaponActor->GetInstigator())
		if (OwningPawn->IsLocallyControlled())
			WeaponEffects->PlayCameraShake();
}

void USWeaponShooter::PlayCommonEffects() const
{
	WeaponEffects->PlayMuzzleEffect();
}