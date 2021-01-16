#include "AI/STrackerBot.h"
#include "Common/Components/SHealthComponent.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


ASTrackerBot::ASTrackerBot()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetCanEverAffectNavigation(false);
	StaticMeshComp->SetSimulatePhysics(true);
	RootComponent = StaticMeshComp;
}

void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	// Initial move-to
	NextPathPoint = CalculateNextPathPoint();
}

void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToTargetByForce();
}

FVector ASTrackerBot::CalculateNextPathPoint() const
{
	AActor* GoalActor = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!GoalActor)
		return GetActorLocation();
	 
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(), GetActorLocation(), GoalActor);

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ASTrackerBot::MoveToTargetByForce()
{
	const bool bIsAtGoal = FVector::Distance(GetActorLocation(), NextPathPoint) < AcceptanceRadiusToTarget;
	if (bIsAtGoal)
	{
		NextPathPoint = CalculateNextPathPoint();
	}
	else
	{
		const FVector ForceDirection = (NextPathPoint - GetActorLocation()).GetSafeNormal();
		const FVector Force = ForceDirection * RollingForceStrength;
		StaticMeshComp->AddForce(Force, NAME_None, bAccelChange);
	}
}
