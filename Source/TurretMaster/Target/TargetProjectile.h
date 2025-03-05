// CG Spectrum 2025

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "TurretMaster/Interface/Targetable.h"
#include "TargetProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class TURRETMASTER_API ATargetProjectile : public AActor, public ITargetable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetProjectile();
	virtual void Tick(float DeltaTime) override;

	/** ITargeteable */
	FORCEINLINE virtual bool IsTargetable_Implementation() const override { return true; }
	FORCEINLINE virtual FGameplayTagContainer GetTargetTags_Implementation() const override { return TargetTags; }
	virtual FTargetData GetTargetData_Implementation() const override;
	virtual FOnTargetDestroyedDelegate& GetOnTargetDestroyed() override { return OnTargetDestroyed; }
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	bool bDrawDebug;

	UFUNCTION(BlueprintCallable)
	void Death();

	// TODO: split this up into two tags, one for type and one for faction, and add the type to the FTargetData Struct
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories="Target,TargetType"))
	FGameplayTagContainer TargetTags;

	FOnTargetDestroyedDelegate OnTargetDestroyed;
	
	// Components
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BaseMesh;
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollider;
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

private:
	UFUNCTION()
	void PredictAndVisualizeProjectilePath() const;
};

