#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "STrackerBot.generated.h"

class USHealthComponent;
class USphereComponent;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, Category=Visual)
	UStaticMeshComponent* StaticMeshComp;

	// when player enters this sphere, the bot will explode
	UPROPERTY(VisibleDefaultsOnly, Category="Damage|ExplodeNearPlayer")
	USphereComponent* PlayerOverlapComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage|ExplodeNearPlayer")
	float SelfHarmRate = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage|ExplodeNearPlayer")
	float SelfHarmDamage = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage|ExplodeNearPlayer")
	USoundBase* SelfDestructionSequenceStartedSound;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Damage")
	USHealthComponent* HealthComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FName LastTimeDamagedParameterName = TEXT("LastTimeDamaged");

	UPROPERTY(EditDefaultsOnly, Category="Damage|Explosion")
	int32 ExplosionDamage = 50;

	UPROPERTY(EditDefaultsOnly, Category="Damage|Explosion")
	float ExplosionRadius = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category="Damage|Explosion")
	TSubclassOf<UDamageType> ExplosionDamageClass = UDamageType::StaticClass();

	UPROPERTY(EditDefaultsOnly, Category="Damage|Explosion")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category="Damage|Explosion")
	USoundBase* ExplosionSound;	

	// when tracker bot enters this sphere, the bot will gain
	UPROPERTY(VisibleDefaultsOnly, Category="Damage|ExplodeNearPlayer")
	USphereComponent* TrackerBotOverlapComponent;

	UPROPERTY(EditDefaultsOnly, Category="Damage|SwarmBoost")
	float AdditionalDamageModifierForEachNewBot = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category="Damage|SwarmBoost")
	float MaxSwarmDamageModifier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Damage|SwarmBoost")
	FName PowerLevelParameterName = TEXT("PowerLevel");

	// How quickly it will move to next path point
	UPROPERTY(EditDefaultsOnly, Category=Movement)
	float RollingForceStrength = 100.0f;

	// When the target will be in acceptance radius movement will stop
	UPROPERTY(EditDefaultsOnly, Category=Movement)
	float AcceptanceRadiusToTarget = 50.0f;

	// If true, mass will have no effect on movement
	UPROPERTY(EditDefaultsOnly, Category=Movement)
	bool bAccelChange = true;

	// If true, mass will have no effect on movement
	UPROPERTY(BlueprintReadOnly, Category=Movement)
	FVector NextPathPoint = FVector(ForceInitToZero);
	

private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentSwarmDamageModifier)
	float CurrentSwarmDamageModifier = 0;
	
	UPROPERTY(ReplicatedUsing=OnRep_bExploded)
	bool bExploded = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_bSelfDestructionStarted)
	bool bSelfDestructionStarted = false;

	int32 TrackerBotsInRange = 0;
	
	FTimerHandle OverlappedWithPlayerSelfHarm_TimerHandle;

public:
	ASTrackerBot();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const override;
	
	void StartSelfDestructionIfNeeded();
	
private:
	UFUNCTION()
	void HandleHealthChanged(USHealthComponent* _, int32 HealthDelta);
	void PlayEffectsOnDamage();

	UFUNCTION()
	void HandleBeginOverlapOfPlayerOverlapComponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void HandleBeginOverlapOfTrackerBotOverlapComponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	void HandleEndOverlapOfTrackerBotOverlapComponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void RecalculateSwarmDamageModifier();

	FVector CalculateNextPathPoint() const;
	void MoveToTargetByForce();

	void SelfHarmNearPlayer();
	
	void Explode();
	void PlayExplosionEffects() const;
	void ApplyDamageAndSelfDestroy();

	UFUNCTION()
	void OnRep_CurrentSwarmDamageModifier();

	UFUNCTION()
	void OnRep_bSelfDestructionStarted() const;
	
	UFUNCTION()
	void OnRep_bExploded() const;
};
