// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health)
	int32 MaxHealthPoints = 100;

	UPROPERTY(BlueprintReadOnly, Category=Health)
	int32 CurrentHealthPoints = MaxHealthPoints;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, USHealthComponent*, HealthComp, int32, HealthDelta);
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnHealthChanged;
	
public:
	USHealthComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);
};
