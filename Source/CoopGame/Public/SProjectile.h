#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class COOPGAME_API ASProjectile : public AActor
{
	GENERATED_BODY()

protected:

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effects)
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	TSubclassOf<UDamageType> DamageType = UDamageType::StaticClass();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	float BaseDamage = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Damage)
	float DamageRadius = 50.0f;


public:
	ASProjectile();

	virtual void BeginPlay() override;
	/** called when projectile hits something */
	UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	void ExplodeWithEffects();
	void PlayExplosionEffects() const;
	void Explode();
};
