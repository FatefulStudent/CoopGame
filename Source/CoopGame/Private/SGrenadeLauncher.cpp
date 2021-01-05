#include "SGrenadeLauncher.h"



void ASGrenadeLauncher::Shoot(const FVector& TraceStart, const FVector& TraceEnd, const FVector& ShotDirection)
{
	APawn* PawnActor = Cast<APawn>(GetOwner());
	// try and fire a projectile
	if (ProjectileClass)
	{
		// Grabs location from the mesh that must have a socket called "Muzzle" in his skeleton
		const FVector MuzzleLocation = SkeletalMeshComp->GetSocketLocation(MuzzleSocketName);
		// Use controller rotation which is our view direction in first person
		const FRotator& MuzzleRotation = PawnActor->GetControlRotation();

		//Set Spawn Collision Handling Override
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwner();
		SpawnParameters.Instigator = GetInstigator();
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<ASProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParameters);
	}
	
	PlayMuzzleEffect();
}
