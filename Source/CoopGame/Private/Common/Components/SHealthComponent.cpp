#include "Common/Components/SHealthComponent.h"
#include "Helpers/NetworkHelper.h"

#include "Net/UnrealNetwork.h"


USHealthComponent::USHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

int32 USHealthComponent::GetMaxHealthPoints() const
{
	return MaxHealthPoints;
}

int32 USHealthComponent::GetCurrentHealthPoints() const
{
	return CurrentHealthPoints;
}

void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealthPoints = MaxHealthPoints;

	if (FNetworkHelper::HasAuthority(this))
	{
		if (AActor* Owner = GetOwner())
		{
			Owner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::OnOwnerTakeDamage);
		}
	}
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, CurrentHealthPoints);
}

void USHealthComponent::OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                          AController* InstigatedBy, AActor* DamageCauser)
{
	check(FNetworkHelper::HasAuthority(this));
	
	if (Damage <= 0.0f)
		return;

	const int32 HealthPointsDelta = -Damage;
	CurrentHealthPoints += HealthPointsDelta;
	const FString OwnerName = GetOwner() ? GetOwner()->GetName() : "Null owner";
	UE_LOG(LogTemp, Log, TEXT("%s was damaged by %s. Remaining health: %d"),
		*OwnerName, *FString::SanitizeFloat(Damage), CurrentHealthPoints);

	OnHealthChanged.Broadcast(this, HealthPointsDelta);
}

void USHealthComponent::OnRep_CurrentHealthPoints(int32 PreviousHealthPointsValue)
{
	const int32 HealthPointsDelta = CurrentHealthPoints - PreviousHealthPointsValue;
	OnHealthChanged.Broadcast(this, HealthPointsDelta);
}
