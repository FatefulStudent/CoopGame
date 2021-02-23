#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

UCLASS( ClassGroup=(CommonComponents), meta=(ChildCannotTick) )
class COOPGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, USHealthComponent*, HealthComp, int32, HealthDelta);
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnHealthChanged;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health)
	int32 MaxHealthPoints = 100;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentHealthPoints, Category=Health)
	int32 CurrentHealthPoints = MaxHealthPoints;
	
public:
	USHealthComponent();
	
	int32 GetMaxHealthPoints() const;
	int32 GetCurrentHealthPoints() const;

	UFUNCTION(BlueprintCallable)
	void Heal(int32 HealAmount);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const override;

	UFUNCTION()
	void OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_CurrentHealthPoints(int32 PreviousHealthPointsValue);
};
