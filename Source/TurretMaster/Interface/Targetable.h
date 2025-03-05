// // CG Spectrum, Nic 2025

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "Targetable.generated.h"

USTRUCT(BlueprintType)
struct TURRETMASTER_API FTargetData
{
	GENERATED_BODY()
	
public:
	FTargetData()
		: Location(FVector::ZeroVector)
		, Velocity(FVector::ZeroVector) {}
	
	UPROPERTY(BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
	FVector Velocity = FVector::ZeroVector;

	// Add more as needed
};

UINTERFACE(BlueprintType)
class UTargetable : public UInterface
{
	GENERATED_BODY()
};

/**
 *	ITargetable
 *	Interface Implemented on all targetable actors
 */
class TURRETMASTER_API ITargetable
{
	GENERATED_BODY()

public:
	/*	Returns all the data needed for movement prediction. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targetable")
	FTargetData GetTargetData() const;

	// Determines whether the actor is targetable.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targetable")
	bool IsTargetable() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targetable")
	FGameplayTagContainer GetTargetTags() const;
};
