// CG Spectrum 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetLauncher.generated.h"

class ATargetProjectile;
class UArrowComponent;

UCLASS()
class TURRETMASTER_API ATargetLauncher : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void LaunchTarget();

protected:
	// Components
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* ForwardArrow;

	// Variables
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATargetProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	float MaxSpreadAngle = 5.0f;
};
