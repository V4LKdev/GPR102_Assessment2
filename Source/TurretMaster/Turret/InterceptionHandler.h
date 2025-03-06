// CG Spectrum, Nic 2025

#pragma once

/**
 *	InterceptionHandler
 *	Helper functions to calculate Interception point
 */
class TURRETMASTER_API InterceptionHandler
{
public:

	/*
	 * This function predicts the earliest possible Interception point for two constant and linear moving objects
	 * taking into account the bullet travel time and turret rotation
	 */
	static bool PredictInterceptionPoint(
		const FVector& TurretPos,
		const FRotator& TurretRot,
		const FVector& TargetPos,
		const FVector& TargetVel,
		float ProjectileSpeed,
		float RotateSpeed,
		float TurretRadius,
		FVector& OutInterceptionPoint);
private:
	
	// TODO: change return to give proper success / failure feedback
	static bool QuadraticEquationInterception(
		const FVector& TurretPos,
		const FVector& TargetPos,
		const FVector& TargetVel,
		float ProjectileSpeed,
		float TimeDelay,
		FVector& OutInterceptionPoint);

	
	static float CalculateRotationTime(
		const FVector& CurrentLoc,
		const FRotator& CurrentRot,
		const FVector& TargetLoc,
		float RotateSpeed,
		float AngleTolerance = 1.0f);

	FORCEINLINE static bool PointInRange(const FVector& Point, const FVector& Center, const float Range) { return FVector::Dist(Point, Center) < Range;}
};