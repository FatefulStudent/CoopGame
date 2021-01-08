// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveActor.generated.h"

class URadialForceComponent;
class USHealthComponent;
UCLASS()
class COOPGAME_API ASExplosiveActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(VisibleDefaultsOnly, Category=Explosion)
	URadialForceComponent* RadialForceComp;
	
	UPROPERTY(EditDefaultsOnly, Category=Explosion)
	UMaterial* ExplodedMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category=Explosion)
	UParticleSystem* ExplosionEffect;
	
	UPROPERTY(VisibleDefaultsOnly, Category=Health)
	USHealthComponent* HealthComp;

private:
	bool bDied = false;
	
public:
	ASExplosiveActor();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* _, int32 HealthDelta);

	void Explode();

	void PlayCosmeticExplosionEffects() const;

	
};
