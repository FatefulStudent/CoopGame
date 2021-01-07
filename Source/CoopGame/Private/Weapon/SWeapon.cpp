#include "Weapon/SWeapon.h"
#include "Weapon/Parts/SWeaponClip.h"
#include "CoopGame/CoopGame.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw debug lines when firing hitscan weapon"),
	ECVF_Cheat
);

ASWeapon::ASWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SkeletalMeshComp;
	
	Clip = CreateDefaultSubobject<USWeaponClip>(TEXT("Clip"));
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60.0f / RateOfFire;
}

void ASWeapon::StartFiring()
{
	const float TimePassedSinceLastFire = GetWorld()->GetTimeSeconds() - LastFireTime;
	const float FirstDelay = FMath::Max(0.0f, TimeBetweenShots - TimePassedSinceLastFire);

	GetWorldTimerManager().SetTimer(
		TimerHandle_TimeBetweenShots,
		this,
		&ASWeapon::Fire,
		TimeBetweenShots,
		true,
		FirstDelay);
}

void ASWeapon::StopFiring()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::Reload()
{
	Clip->Reload();
}

void ASWeapon::Fire()
{
	if (!Clip->HasBullets())
	{
		StopFiring();
		return;
	}
	
	// Trace the world from the owners eyes perspective
	if (AActor* OwnerActor = GetOwner())
	{
		FVector TraceStart;
		FRotator EyeRotation;
		OwnerActor->GetActorEyesViewPoint(TraceStart, EyeRotation);

		const FVector ShotDirection = EyeRotation.Vector();
		const FVector TraceEnd = TraceStart + ShotDirection * 10000.0f;

		Shoot(TraceStart, TraceEnd, ShotDirection);
		Clip->SpendBullet();
		LastFireTime = GetWorld()->GetTimeSeconds();
	}
}

void ASWeapon::Shoot(
	const FVector& TraceStart,
	const FVector& TraceEnd,
	const FVector& ShotDirection)
{
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	CollisionQueryParams.AddIgnoredActor(this);
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
		if (HasAuthority())
			ApplyPointDamageToHitActor(ShotDirection, HitResult);

		PlayEffectsOnImpact(HitResult);
	}

	const FVector& TraceEffectEnd = bBlockingHit ? HitResult.ImpactPoint : TraceEnd;

	PlayFireEffects(TraceEffectEnd);

	if (DebugWeaponDrawing > 0)
		DrawDebug(TraceStart, TraceEnd);
}

void ASWeapon::PlayCameraShake() const
{
	if (APlayerController* InstigatorPlayerController = Cast<APlayerController>(GetInstigatorController()))
	{
		InstigatorPlayerController->ClientPlayCameraShake(CameraShakeClass);
	}
}

void ASWeapon::PlayFireEffects(const FVector& TraceEffectEnd) const
{
	PlayCameraShake();
	PlayTraceEffect(TraceEffectEnd);
	PlayMuzzleEffect();
}

void ASWeapon::PlayTraceEffect(const FVector& TraceEffectEnd) const
{
	if (TracerEffect)
	{
		const FVector MuzzleLocation = SkeletalMeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TraceParticlesComp =
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		TraceParticlesComp->SetVectorParameter(TraceTargetName, TraceEffectEnd);
	}
}

void ASWeapon::PlayMuzzleEffect() const
{
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, SkeletalMeshComp, MuzzleSocketName);
}

void ASWeapon::PlayEffectsOnImpact(const FHitResult& HitResult) const
{
	const EPhysicalSurface HitSurfaceType = UGameplayStatics::GetSurfaceType(HitResult);

	UParticleSystem* ImpactEffect = nullptr;

	switch (HitSurfaceType)
	{
	case SURFACE_FLESH_DEFAULT:
		ImpactEffect = FleshImpactEffect;
		break;
	case SURFACE_FLESH_VULNERABLE:
		ImpactEffect = VulnerableFleshImpactEffect;
		break;
	default:
		ImpactEffect = DefaultImpactEffect;
		break;
	}

	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			ImpactEffect,
			HitResult.ImpactPoint,
			HitResult.ImpactNormal.Rotation());
	}
}

void ASWeapon::DrawDebug(const FVector& TraceStart, const FVector& TraceEnd) const
{
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f);
}

void ASWeapon::ApplyPointDamageToHitActor(const FVector& ShotDirection, const FHitResult& HitResult)
{
	float DamageToApply = BaseDamage;

	if (UGameplayStatics::GetSurfaceType(HitResult) == SURFACE_FLESH_VULNERABLE)
		DamageToApply *= VulnerableFleshDamageMultiplier;

	UGameplayStatics::ApplyPointDamage(
		HitResult.GetActor(),
		DamageToApply,
		ShotDirection,
		HitResult,
		GetInstigatorController(),
		this,
		DamageType);
}
