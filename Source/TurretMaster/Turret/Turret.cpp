// CG Spectrum, Nic 2025

#include "Turret.h"

#include "InterceptionHandler.h"
#include "TurretProjectile.h"
#include "Components/SphereComponent.h"
#include "TurretMaster/TurretMasterGameplayTags.h"
#include "TurretMaster/TurretMasterLogChannels.h"
#include "TurretMaster/Interface/Targetable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Turret)

#pragma region Construction

ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;
	
	RotationPoint = CreateDefaultSubobject<USceneComponent>("RotationPoint");
	RotationPoint->SetupAttachment(RootComponent);
	
	ArmMesh = CreateDefaultSubobject<UStaticMeshComponent>("ArmMesh");
	ArmMesh->SetupAttachment(RotationPoint);
	
	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>("CannonMesh");
	CannonMesh->SetupAttachment(ArmMesh);
	
	CentreMuzzle = CreateDefaultSubobject<USceneComponent>("CentreMuzzle");
	CentreMuzzle->SetupAttachment(CannonMesh);

	
	TurretArea = CreateDefaultSubobject<USphereComponent>("TurretArea");
	TurretArea->SetupAttachment(RootComponent);
	TurretArea->SetSphereRadius(Range);
	TurretArea->SetLineThickness(10.f);
	TurretArea->ShapeColor = FColor::Yellow;
	TurretArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TurretArea->SetCollisionProfileName("OverlapAllDynamic");
}

void ATurret::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TurretArea->SetSphereRadius(Range);

	/** DEBUG */
	TurretArea->bHiddenInGame = !bDrawDebug;
}

#pragma endregion // Construction


void ATurret::BeginPlay()
{
	Super::BeginPlay();
	UpdateTickState();

	TurretArea->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	TurretArea->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

#pragma region Overlapping

void ATurret::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	// Called after returning from this function
	ON_SCOPE_EXIT
	{
		UpdateTickState();
	};
	
	// TODO: check if turret is active and the target tag
	if (OtherActor && OtherActor->Implements<UTargetable>())
	{
		if (ActiveTargets.Contains(OtherActor) || !ITargetable::Execute_IsTargetable(OtherActor))
		{
			return;
		}

		const FGameplayTagContainer TargetTags = ITargetable::Execute_GetTargetTags(OtherActor);
		if (!TargetableTags.HasAnyExact(TargetTags))
		{
			return;
		}

		// Binds the Target's OnTargetDestroyed Delegate
		CastChecked<ITargetable>(OtherActor)->GetOnTargetDestroyed().AddLambda([&](AActor* Caller)
		{
			TryRemoveActiveTarget(Caller, true);
		});
		
		UE_LOG(LogTurretMaster, Log, TEXT("Turret %s: Target (%s) Entered Effective Range"), *GetName(), *OtherActor->GetName());
		ActiveTargets.Add(OtherActor);
	}
}

void ATurret::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	TryRemoveActiveTarget(OtherActor);
}

#pragma endregion // Overlapping

#pragma region Target Handling

void ATurret::TryRemoveActiveTarget(AActor* Target, const bool bDestroyed)
{
	if (!Target)
	{
		UE_LOG(LogTurretMaster, Warning, TEXT("%s: Attempting to remove an invalid Target"), *GetName());
		return;
	}

	// We assume a target here has to implement the Interface
	ITargetable* Targetable = CastChecked<ITargetable>(Target);
	/** UNBIND DELEGATES */
	Targetable->GetOnTargetDestroyed().Clear(); // TODO: change this to only remove bind to self if not destroyed, since it can still be destroyed by another turret
	
	if (ActiveTargets.Contains(Target))
	{
		ActiveTargets.Remove(Target);
		
	}
	
	if (Target == CurrentTarget.Key)
	{
		CurrentTarget.Key = nullptr;
		CurrentTarget.Value = FGameplayTag();
		UE_LOG(LogTurretMaster, Warning, TEXT("%s: Removing Active Target"), *Target->GetName());
	}

	UpdateTickState();

	if (bDestroyed)
	{
		// TODO: Increment counter
	}
}

void ATurret::SetTarget()
{
	// Iterate over active targets until a valid target is found
	for (int i = 0; i < ActiveTargets.Num(); i++)
	{
		AActor* Target = ActiveTargets[i];
		CurrentTarget.Key = Target;
		if (!DetermineTargetType(*Target, CurrentTarget.Value))
		{
			UE_LOG(LogTurretMaster, Warning, TEXT("Target %s does not have a 'TargetType' tag, Defaulting to GravityProjectile"), *Target->GetName());
			CurrentTarget.Value = TMGameplayTags::TargetType_Projectile_Gravity;
		}

		// Here we can do some target validation logic later, e.g. check if its already being targeted by another turret
		if (true) // Dummy Condition
		{
			break;
		}
	}
}

bool ATurret::DetermineTargetType(const AActor& Target, FGameplayTag& TargetType)
{
	bool bFoundTag = false;
	int MatchingTagCount = 0;

	FGameplayTagContainer TagContainer = ITargetable::Execute_GetTargetTags(&Target);
	
	for (const FGameplayTag& Tag : TagContainer)
	{
		if (Tag.MatchesTag(TMGameplayTags::TargetType))
		{
			TargetType = Tag;
			bFoundTag = true;
			MatchingTagCount++;
		}
	}

	if (MatchingTagCount > 1)
	{
		UE_LOG(LogTurretMaster, Error, TEXT("Found several 'TargetType' tags on %s when expecting only one"), *Target.GetName());
	}
	
	return bFoundTag;
}

#pragma endregion // Target Handling


void ATurret::UpdateTickState()
{
	// If there are any active targets, enable tick to trigger targeting logic
	bool bShouldTickActor;
	const bool bHasTargets = ActiveTargets.IsEmpty();
	bShouldTickActor = !bHasTargets; // TODO: Implement Idle state animation case here
	SetActorTickEnabled(bShouldTickActor);

	/** DEBUG */
	if (bHasTargets)
	{
		TurretArea->ShapeColor = FColor::Green;
		TurretArea->MarkRenderStateDirty();
	}
	else
	{
		TurretArea->ShapeColor = FColor::Yellow;
		TurretArea->MarkRenderStateDirty();
	}
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* TODO: Determine Target
		That includes checking
		TargetType
		If its still Reachable
		If its already being targeted */

	if (ActiveTargets.IsEmpty())
	{
		// TODO: Perform move to Idle here, then disable tick
		MoveToIdle();
	}
	
	if (CurrentTarget.Key == nullptr)
	{
		// Select a target if none is currently set, this should always return a valid target as @CurrentTarget
		SetTarget();
		check(CurrentTarget.Key);
		UE_LOG(LogTurretMaster, Log, TEXT("Active Target Changed"));

		// TODO: Calculate impact point

		FVector InterceptionPoint = FVector::ZeroVector;
		const FTargetData TargetData = ITargetable::Execute_GetTargetData(CurrentTarget.Key);
		bool bValidInterceptionPoint = false;

		// Calculate Collision Point depending on what type of target it is
		if (CurrentTarget.Value.MatchesTagExact(TMGameplayTags::TargetType_Stationary))
		{
			bValidInterceptionPoint = CalculateInterceptionPoint_Stationary(TargetData, InterceptionPoint);
		}
		else if (CurrentTarget.Value.MatchesTagExact(TMGameplayTags::TargetType_Projectile_NoGravity))
		{
			bValidInterceptionPoint = CalculateInterceptionPoint_ProjectileNoGravity(TargetData, InterceptionPoint);
		}
		else if (CurrentTarget.Value.MatchesTagExact(TMGameplayTags::TargetType_Projectile_Gravity))
		{
			bValidInterceptionPoint = CalculateInterceptionPoint_ProjectileGravity(TargetData, InterceptionPoint);
		}

		if (!bValidInterceptionPoint)
		{
			// If the collision point is deemed invalid or unreachable, remove the target from the active list.
			TryRemoveActiveTarget(CurrentTarget.Key, false);
		}
	}
	
	// TODO: Set yaw and pitch

	// TODO: Check muzzle is pointed at impact point

	// TODO: If it is, FIRE!
}



void ATurret::MoveToIdle()
{
}



bool ATurret::CalculateInterceptionPoint_ProjectileNoGravity(const FTargetData& TargetData, FVector& InterceptionPoint)
{
#if 0
	const float TimeStep = 0.1f;
	const int32 NumSteps = 50;
	const float PredictionDuration = TimeStep * NumSteps;

	TArray<FVector> PredictionPoints;
	PredictionPoints.Reserve(NumSteps);

	for (int i = 0; i < NumSteps; i++)
	{
		const float CurrentTime = TimeStep * i;
		const FVector PredictedPosition = TargetData.Location + (TargetData.Velocity * CurrentTime);
		PredictionPoints.Add(PredictedPosition);
	}

	if (bDrawDebug)
	{
		UWorld* World = GetWorld();
		for (int32 i = 0; i < PredictionPoints.Num() - 1; ++i)
		{
			// Draw line between consecutive points
			DrawDebugLine(
				World,
				PredictionPoints[i],
				PredictionPoints[i + 1],
				FColor::Green,
				false, 5.0f, 0,
				1.5f // Thickness
			);
        
			// Draw point at prediction location
			DrawDebugPoint(
				World,
				PredictionPoints[i],
				5,
				FColor::Red,
				false,
				5.f
			);
		}
	}
#endif

	InterceptionPoint = InterceptionHandler::QuadraticEquationInterception(CentreMuzzle->GetComponentLocation(), TargetData.Location, TargetData.Velocity, 2500.f);

	if (InterceptionPoint == FVector::ZeroVector || !IsPointWithinRange(InterceptionPoint))
	{
		UE_LOG(LogTurretMaster, Warning, TEXT("No Interception Possible"));
	}
	
	else if (bDrawDebug)
	{
		DrawDebugLine(GetWorld(), CentreMuzzle->GetComponentLocation(), InterceptionPoint, FColor::Red, false, 5.0f, 0, 1.5f);
		DrawDebugSphere(GetWorld(), InterceptionPoint, 25.f, 12,FColor::Orange, false, 5.0f, 0, 1.5f);
	}
	
	return true;
}

bool ATurret::CalculateInterceptionPoint_ProjectileGravity(const FTargetData& TargetData, FVector& InterceptionPoint)
{
	return true;
}

bool ATurret::CalculateInterceptionPoint_Stationary(const FTargetData& TargetData, FVector& InterceptionPoint) const
{
	InterceptionPoint = TargetData.Location;
	return IsPointWithinRange(InterceptionPoint);
}

bool ATurret::IsPointWithinRange(const FVector& Point) const
{
	float Distance = FVector::Dist(Point, GetActorLocation());
	return Distance <= TurretArea->GetScaledSphereRadius();
}




void ATurret::Fire() const
{
	// Spawn Parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn the turret projectile
	const AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, CentreMuzzle->GetComponentLocation(), CentreMuzzle->GetComponentRotation(), SpawnParams);
	if (SpawnedProjectile)
	{
		UE_LOG(LogTurretMaster, Warning, TEXT("Turret Projectile spawned."));
	}
}

void ATurret::SetYaw(const float TargetYaw) const
{
	const FRotator CurrentRotation = RotationPoint->GetComponentRotation();
	const float NewYaw = FMath::FInterpTo(CurrentRotation.Yaw, TargetYaw, GetWorld()->GetDeltaSeconds(), TurnSpeed);
	RotationPoint->SetWorldRotation(FRotator(CurrentRotation.Pitch, NewYaw, CurrentRotation.Roll));
}

void ATurret::SetPitch(const float TargetPitch) const
{
	const FRotator CurrentRotation = RotationPoint->GetComponentRotation();
	const float NewPitch = FMath::FInterpTo(CurrentRotation.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), TurnSpeed);
	RotationPoint->SetWorldRotation(FRotator(NewPitch, CurrentRotation.Yaw, CurrentRotation.Roll));
}


