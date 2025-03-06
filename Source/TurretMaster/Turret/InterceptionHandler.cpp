// CG Spectrum, Nic 2025

#include "InterceptionHandler.h"

#include "TurretMaster/TurretMasterLogChannels.h"

FVector InterceptionHandler::PredictInterceptionPoint(
	const FVector& TurretPos,
	const FRotator& TurretRot,
	const FVector& TargetPos,
	const FVector& TargetVel,
	float ProjectileSpeed,
	float RotateSpeed)
{
	/* Calculates the interception point based on projectile speed and turret rotation speed in an iterative approach */

	// 1. Calculate initial interception point ignoring turret rotation.
	FVector InitialIntercept = QuadraticEquationInterception(TurretPos, TargetPos, TargetVel, ProjectileSpeed);
	if(InitialIntercept.IsZero())
	{
		return FVector::ZeroVector;
	}

	// 2. Compute rotation delay based on the initial predicted intercept.
	float RotationTime = CalculateRotationTime(TurretPos, TurretRot, InitialIntercept, RotateSpeed);
	RotationTime += 0.08f; // Little extra room for calculation errors, spawn delay etc, TODO: Change to Constant Later
	
	// 3. Calculate Interception now with the TimeDelay
	FVector InterceptionPoint = QuadraticEquationInterception(TurretPos, TargetPos, TargetVel, ProjectileSpeed, RotateSpeed);
	if (InterceptionPoint.IsZero())
	{
		return FVector::ZeroVector; // TODO: properly handle impossible interceptions
	}

	return InterceptionPoint;
}

TArray<FVector> InterceptionHandler::CalculateInterceptionWindow(
	const FVector& TurretPos,
	const FRotator& TurretRot,
	const FVector& TargetPos,
	const FVector& TargetVel,
	float ProjectileSpeed,
	float RotateSpeed,
	float TurretRadius,
	int32 NumberOfSteps,
	float TimeStep)
{

	TArray<FVector> ValidPoints;
	
	for (int i = 0; i < NumberOfSteps; i++)
	{
		const float CurrentTime = TimeStep * i;
		
		// Advance target position by current sample time
		const FVector FutureTargetPos = TargetPos + (TargetVel * CurrentTime);
		
		// Calculate interception point for this future position
		const FVector InterceptionPoint = QuadraticEquationInterception(
			TurretPos,
			FutureTargetPos,
			TargetVel,
			ProjectileSpeed
		);

		if (InterceptionPoint != FVector::ZeroVector && FVector::Dist(InterceptionPoint, TurretPos) < TurretRadius)
		{
			ValidPoints.Add(InterceptionPoint);
		}
		else if (ValidPoints.Num() > 2)
		{
			// Optimize to not always calculate all points if the window has passed already
			UE_LOG(LogInterception, Log, TEXT("Exiting Prediction Early"));
			break;
		}
	}

	return ValidPoints;
}



FVector InterceptionHandler::QuadraticEquationInterception(const FVector& TurretPos, const FVector& TargetPos, const FVector& TargetVel, float ProjectileSpeed, float TimeDelay)
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
	
	// Relative vector from turret to target's CURRENT position Or Future with the delay for turret rotation in mind
	const FVector ActualTargetPos = TargetPos + TargetVel * TimeDelay;
	const FVector D = ActualTargetPos - TurretPos;
	
	// Squared projectile speed and target velocity
	const float PsSq = FMath::Square(ProjectileSpeed);
	const float VtSq = TargetVel.Dot(TargetVel);

	// Quadratic equation coefficients
	const float a = VtSq - PsSq;								// t² coefficient
	const float b = 2 * FVector::DotProduct(D, TargetVel);	// t coefficient
	const float c = D.Dot(D);									// Constant term


	/* EDGE CASES */

	//	Target is (Nearly) Stationary
	if (TargetVel.IsNearlyZero(KINDA_SMALL_NUMBER))
	{
		return ActualTargetPos;
	}
	
	//	Target is Directly at Turret Position
	if (D.IsNearlyZero(KINDA_SMALL_NUMBER))
	{
		return (ProjectileSpeed > 0) ? ActualTargetPos : FVector::ZeroVector;
	}
	
	// Target and Projectile move at same velocity
	if (FMath::IsNearlyZero(a, KINDA_SMALL_NUMBER))
	{
		// Linear equation: b*t + c = 0 → t = -c/b
		if (FMath::IsNearlyZero(b, KINDA_SMALL_NUMBER))
		{
			// Both a=0 and b=0
			return (c < KINDA_SMALL_NUMBER) ? ActualTargetPos : FVector::ZeroVector;
		}
        
		const float t = -c / b;
		if (t > 0)
		{
			return ActualTargetPos + TargetVel * t;
		}
		return FVector::ZeroVector; // No future interception
	}
	/* END EDGE CASES */
	
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

	return ActualTargetPos + (TargetVel * ValidTime);
}

float InterceptionHandler::CalculateRotationTime(const FVector& CurrentLoc, const FRotator& CurrentRot,
	const FVector& TargetLoc, float RotateSpeed, float AngleTolerance)
{
	const FVector TargetDir = (TargetLoc - CurrentLoc).GetSafeNormal();
	const FRotator TargetRot = TargetDir.Rotation();

	// Calculate angular differences
	const float YawDiff = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, TargetRot.Yaw);
	const float PitchDiff = FMath::FindDeltaAngleDegrees(CurrentRot.Pitch, TargetRot.Pitch);
	const float MaxAngularDistance = FMath::Max(FMath::Abs(YawDiff), FMath::Abs(PitchDiff));

	if (MaxAngularDistance <= AngleTolerance) return 0.f;
	if (RotateSpeed <= 0.f) return BIG_NUMBER;

	return FMath::Max(0.f, FMath::Loge(MaxAngularDistance / AngleTolerance) / RotateSpeed);
}

