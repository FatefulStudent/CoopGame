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
	USphereComponent* OverlapComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage|ExplodeNearPlayer")
	float SelfHarmRate = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage|ExplodeNearPlayer")
	float SelfHarmDamage = 20.0f;
	
	UPROPERTY(VisibleDefaultsOnly, Category=Damage)
	USHealthComponent* HealthComp;
	
	UPROPERTY(EditDefaultsOnly, Category=Damage)
	FName LastTimeDamagedParameterName = TEXT("LastTimeDamaged");

	UPROPERTY(EditDefaultsOnly, Category=Damage)
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category=Damage)
	int32 ExplosionDamage = 50;

	UPROPERTY(EditDefaultsOnly, Category=Damage)
	float ExplosionRadius = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category=Damage)
	TSubclassOf<UDamageType> ExplosionDamageClass = UDamageType::StaticClass();

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
	UMaterialInstanceDynamic* MaterialForPulseOnDamage;

	bool bExploded = false;

	FTimerHandle OverlappedWithPlayerSelfHarm_TimerHandle;

public:
	ASTrackerBot();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	

private:
	UFUNCTION()
	void HandleHealthChanged(USHealthComponent* _, int32 HealthDelta);
	void PlayEffectsOnDamage();

	FVector CalculateNextPathPoint() const;
	void MoveToTargetByForce();

	void SelfHarmNearPlayer();
	
	void Explode();
	void PlayExplosionEffects() const;
	void ApplyDamageAndSelfDestroy();
};
