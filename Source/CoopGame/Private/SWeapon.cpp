#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ASWeapon::ASWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SkeletalMeshComp;
}

void ASWeapon::Fire()
{
	// Trace the world from the owners eyes perspective

	if (AActor* OwnerActor = GetOwner())
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		const FVector ShotDirection = EyeRotation.Vector();
		const FVector TraceEnd = EyeLocation + ShotDirection * 10000.0f;
		
		ShootAndDamageHitActor(OwnerActor, EyeLocation, TraceEnd, ShotDirection);
	}
}

void ASWeapon::ShootAndDamageHitActor(AActor* OwnerActor,
	const FVector& TraceStart,
	const FVector& TraceEnd,
	const FVector& ShotDirection)
{
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(OwnerActor);
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bTraceComplex = true;

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility))
	{
		UGameplayStatics::ApplyPointDamage(HitResult.GetActor(),
            20.0,
            ShotDirection,
            HitResult, 
            GetInstigatorController(), 
            this,
            DamageType);
			
	}

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 10.0f);
}
