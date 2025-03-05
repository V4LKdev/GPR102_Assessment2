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
	//static FInterceptionSolution CalculateLinearInterceptionSolution(
	//const FVector& TurretLocation,
	//const FRotator& TurretCurrentRotation,
	//float TurretTurnSpeed,
	//float ProjectileSpeed,
	//const FVector& TargetLocation,
	//const FVector& TargetVelocity);

public:
	static FVector QuadraticEquationInterception(
		const FVector& TurretPos,
		const FVector& TargetPos,
		const FVector& TargetVel,
		float ProjectileSpeed);
};