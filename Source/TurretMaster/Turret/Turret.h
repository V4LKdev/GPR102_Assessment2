// CG Spectrum 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class ATurretProjectile;

UCLASS()
class TURRETMASTER_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Fire() const;

	UFUNCTION()
	void SetYaw(float TargetYaw) const;
	
	UFUNCTION()
	void SetPitch(float TargetPitch) const;

protected:
	// Components
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* RotationPoint;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ArmMesh;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* CannonMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* CentreMuzzle;
	
	// Variables
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATurretProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	float TurnSpeed = 5.0f;
};
