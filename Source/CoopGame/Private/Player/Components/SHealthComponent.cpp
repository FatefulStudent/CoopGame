#include "Player/Components/SHealthComponent.h"


USHealthComponent::USHealthComponent()
{
	
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

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::OnOwnerTakeDamage);
	}
}

void USHealthComponent::OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
		return;

	CurrentHealthPoints -= Damage;
	UE_LOG(LogTemp, Log, TEXT("%s was damaged by %s. Remaining health: %d"),
		*GetName(), *FString::SanitizeFloat(Damage), CurrentHealthPoints);

	OnHealthChanged.Broadcast(this, Damage);
}
