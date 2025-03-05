﻿// CG Spectrum, Nic 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretMaster/Interface/Targetable.h"
#include "TurretProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class TURRETMASTER_API ATurretProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurretProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollider;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovement;
};
