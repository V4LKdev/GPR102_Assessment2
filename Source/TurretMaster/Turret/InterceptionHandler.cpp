// CG Spectrum, Nic 2025

#include "InterceptionHandler.h"

#include "TurretMaster/TurretMasterLogChannels.h"

FVector InterceptionHandler::QuadraticEquationInterception(const FVector& TurretPos, const FVector& TargetPos, const FVector& TargetVel, float ProjectileSpeed)
{
	/**
	 * Calculates the interception point to hit a linear moving target with a linear projectile.
	 * 
	 * Theory:
	 * Find time 't' where both the projectile and target reach the same point.
	 * 
	 * Equation Derivation:
	 * - Let TurretPos = S, TargetPos = T0, TargetVel = Vt, ProjectileSpeed = Ps
	 * - Target position at time t: T(t) = T0 + Vt * t
	 * - Projectile must reach T(t) in time t: Distance(S, T(t)) = Ps * t
	 * - Square both sides: (T0 - S + Vt*t) ⋅ (T0 - S + Vt*t) = (Ps*t)^2
	 * - Expand to quadratic form: (Vt⋅Vt - Ps²)t² + 2(T0 - S)⋅Vt t + (T0 - S)⋅(T0 - S) = 0
	 * 
	 * Coefficients:
	 * a = Vt⋅Vt - Ps²
	 * b = 2 * (T0 - S)⋅Vt
	 * c = (T0 - S)⋅(T0 - S)
	 */
	
	// Relative vector from turret to target's CURRENT position
	const FVector D = TargetPos - TurretPos;
	
	// Squared projectile speed and target velocity
	const float PsSq = FMath::Square(ProjectileSpeed);
	const float VtSq = TargetVel.Dot(TargetVel);

	// Quadratic equation coefficients
	const float a = VtSq - PsSq;								// t² coefficient
	const float b = 2 * FVector::DotProduct(D, TargetVel);	// t coefficient
	const float c = D.Dot(D);									// Constant term
	
	const float Discriminant = b * b - 4 * a * c;

	// No real solution = impossible interception
	if (Discriminant < 0)
	{
		return FVector::ZeroVector;
	}

	// Calculate both roots
	const float SqrtD = FMath::Sqrt(Discriminant);
	const float t1 = (-b + SqrtD) / (2 * a);
	const float t2 = (-b - SqrtD) / (2 * a);

	// Find the smallest positive time
	float ValidTime = 0;
	if (t1 > 0 && t2 > 0) ValidTime = FMath::Min(t1, t2);
	else if (t1 > 0) ValidTime = t1;
	else if (t2 > 0) ValidTime = t2;
	else return FVector::ZeroVector; // No valid time in the future

	return TargetPos + (TargetVel * ValidTime);
}
