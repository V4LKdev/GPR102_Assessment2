// CG Spectrum 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class TURRETMASTER_API ATargetProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Components
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollider;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovement;
};
