#include "SWeapon.h"

ASWeapon::ASWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SkeletalMeshComp;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

