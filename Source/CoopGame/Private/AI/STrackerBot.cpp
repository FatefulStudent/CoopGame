#include "AI/STrackerBot.h"


ASTrackerBot::ASTrackerBot()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetCanEverAffectNavigation(false);
	RootComponent = StaticMeshComp;
}

void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
