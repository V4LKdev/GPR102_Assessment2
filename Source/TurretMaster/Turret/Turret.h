// CG Spectrum, Nic 2025

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class UTargetable;
class USphereComponent;
class ATurretProjectile;

UCLASS()
class TURRETMASTER_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	ATurret();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Fire() const;
	UFUNCTION()
	void SetYaw(float TargetYaw) const;
	UFUNCTION()
	void SetPitch(float TargetPitch) const;

protected:
	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
	
	// Components
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> BaseMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> RotationPoint;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> ArmMesh;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> CannonMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> CentreMuzzle;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USphereComponent> TurretArea;
	
	// Variables
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATurretProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	float TurnSpeed = 5.0f;
	UPROPERTY(EditAnywhere)
	float Range = 5000.f;
	UPROPERTY(EditAnywhere, meta = (Categories="Target", ToolTip="What targets the turrent shoots at"))
	FGameplayTagContainer Targets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Development")
	bool bDrawDebug = true;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	TArray<AActor*> ActiveTargets;
	
	//FGameplayTag CurrentTurretState;
	//
	//UFUNCTION(BlueprintCallable, meta=(Categories="TurretState"))
	//FORCEINLINE void SetTurretState(const FGameplayTag NewState) { CurrentTurretState = NewState; }
	//UFUNCTION(BlueprintCallable)
	//FGameplayTag GetCurrentTurretState() const { return CurrentTurretState; }
};