// CG Spectrum, Nic 2025

#pragma once

struct FInterceptionSolution
{
	bool bHasValidSolution;
	FVector ImpactPoint;
	
	float TimeToImpact;
	float TurretRotationDuration;
	float ProjectileTravelTime;
};

/**
 *	InterceptionHandler
 *	Helper functions to calculate Interception point
 */
class TURRETMASTER_API InterceptionHandler
{
public:

	/*
	 * This function predicts the earliest possible Interception point
	 */
	static FVector PredictInterceptionPoint(
		const FVector& TurretPos,
		const FRotator& TurretRot,
		const FVector& TargetPos,
		const FVector& TargetVel,
		float ProjectileSpeed,
		float RotateSpeed);

	static TArray<FVector> CalculateInterceptionWindow(
		const FVector& TurretPos,
		const FRotator& TurretRot,
		const FVector& TargetPos,
		const FVector& TargetVel,
		float ProjectileSpeed,
		float RotateSpeed,
		float TurretRadius,
		int32 NumberOfSteps = 80,
		float TimeStep = 0.075f);

private:
	
	// TODO: change return to give proper success / failure feedback
	static FVector QuadraticEquationInterception(
		const FVector& TurretPos,
		const FVector& TargetPos,
		const FVector& TargetVel,
		float ProjectileSpeed,
		float TimeDelay = 0.f);

	
	static float CalculateRotationTime(
		const FVector& CurrentLocation,
		const FRotator& CurrentRotation,
		const FVector& TargetLocation,
		float RotateSpeed,
		float AngleTolerance = 1.0f);
	
};