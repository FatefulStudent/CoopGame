#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveActor.generated.h"

class URadialForceComponent;
class USHealthComponent;
UCLASS( ClassGroup=(LevelObjects), meta=(ChildCannotTick) )
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
	
	UPROPERTY(EditDefaultsOnly, Category=Explosion)
	float ExplosionDamage = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, Category=Explosion)
	TSubclassOf<UDamageType> ExplosionDamageClass = UDamageType::StaticClass();
	
	UPROPERTY(VisibleDefaultsOnly, Category=Health)
	USHealthComponent* HealthComp;

private:
	UPROPERTY(ReplicatedUsing=OnRep_bExploded)
	bool bExploded = false;
	
public:
	ASExplosiveActor();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const override;

private:
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* _, int32 HealthDelta);

	void Explode();

	void PlayCosmeticExplosionEffects() const;

	UFUNCTION()
	void OnRep_bExploded() const;
};
