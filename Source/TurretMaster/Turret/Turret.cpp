// CG Spectrum, Nic 2025

#include "Turret.h"

#include "InterceptionHandler.h"
#include "KismetTraceUtils.h"
#include "TurretProjectile.h"
#include "Chaos/PBDSuspensionConstraintData.h"
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
	
	CenterMuzzle = CreateDefaultSubobject<USceneComponent>("CenterMuzzle");
	CenterMuzzle->SetupAttachment(CannonMesh);

	
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
			TryRemoveActiveTarget(Caller);
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
	if (OtherActor && OtherActor->Implements<UTargetable>())
	{
		TryRemoveActiveTarget(OtherActor);
	}
}

#pragma endregion // Overlapping

#pragma region Target Handling

void ATurret::TryRemoveActiveTarget(AActor* Target)
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
}

void ATurret::DetermineNewTarget()
{
	/* Iterate over active targets until a valid target is found
	* For now this will always return a valid target */
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

	check(CurrentTarget.Key); // Ensure we got a valid target to work with

	// Calculate Interception point based on the projectile type
	FVector InterceptionPoint = FVector::ZeroVector;
	TargetInterceptionRotation = FRotator::ZeroRotator;
	bool bValidInterceptionPoint = false;
	const FTargetData TargetData = ITargetable::Execute_GetTargetData(CurrentTarget.Key);
	
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
		//TODO 
		//bValidInterceptionPoint = CalculateInterceptionPoint_ProjectileGravity(TargetData, InterceptionPoint);
		bValidInterceptionPoint = false;
	}
	else
	{
		UE_LOG(LogTurretMaster, Warning, TEXT("Unhandled TargetType: %s"), *CurrentTarget.Value.GetTagName().ToString());
		bValidInterceptionPoint = false;
	}

	if (!bValidInterceptionPoint || !IsPointWithinRange(InterceptionPoint))
	{
		// If the collision point is deemed invalid or unreachable, remove the target from the active list and find a new target
		TryRemoveActiveTarget(CurrentTarget.Key);
		if (!ActiveTargets.IsEmpty())
		{
			DetermineNewTarget();
		}
	}

	// Calculate Target Rotation
	FVector Direction = InterceptionPoint - RotationPoint->GetComponentLocation();
	Direction.Normalize();
	TargetInterceptionRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	
	UE_LOG(LogTurretMaster, Verbose, TEXT("Active Target Changed"));
}

bool ATurret::DetermineTargetType(const AActor& Target, FGameplayTag& TargetType)
{
	bool bFoundTag = false;
	int MatchingTagCount = 0;

	// This should be valid for all targets, but maybe handle failure here
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
	const bool bHasTargets = !ActiveTargets.IsEmpty();
	const bool bInIdlePos = RotationPoint->GetComponentRotation().Equals(GetIdleRotation(), 1.f);
	const bool bIsTicking = IsActorTickEnabled();

	if (!bIsTicking &&(bHasTargets || (!bHasTargets && !bInIdlePos)))
	{
		SetActorTickEnabled(true);
	}
	else if (bIsTicking && !bHasTargets && bInIdlePos)
	{
		SetActorTickEnabled(false);
	}

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
	
	if (ActiveTargets.IsEmpty())
	{
		MoveToIdle();
		return;
	}
	
	if (CurrentTarget.Key == nullptr)
	{
		DetermineNewTarget();
	}
	
	// If a target is set with a valid Interception point, rotate towards that point
	if (CurrentTarget.Key != nullptr && TargetInterceptionRotation != FRotator::ZeroRotator)
	{
		if (!RotationPoint->GetComponentRotation().Equals(TargetInterceptionRotation, 0.5f))
		{
			SetPitch(TargetInterceptionRotation.Pitch);
			SetYaw(TargetInterceptionRotation.Yaw);

			return;
		}

		// TODO: Check muzzle is pointed at impact point

		Fire();
		TryRemoveActiveTarget(CurrentTarget.Key);
	}
}

// TODO: Unify rotation into one function
void ATurret::MoveToIdle()
{
	SetPitch(GetIdleRotation().Pitch);
	SetYaw(GetIdleRotation().Yaw);

	// Disable Tick if IdleRotation is reached
	UpdateTickState();
}


bool ATurret::CalculateInterceptionPoint_ProjectileNoGravity(const FTargetData& TargetData, FVector& InterceptionPoint)
{
	float Radius = TurretArea->GetScaledSphereRadius();
	
	bool bIsInterceptable = InterceptionHandler::PredictInterceptionPoint(
		CenterMuzzle->GetComponentLocation(),
		RotationPoint->GetComponentRotation(),
		TargetData.Location,
		TargetData.Velocity,
		3000.f /* TODO: get ProjectileSpeed */,
		TurnSpeed,
		Radius,
		InterceptionPoint);

	if (bIsInterceptable)
	{

		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(),
				InterceptionPoint,
				15.f,
				12,
				FColor::Emerald,
				false,
				3.f,
				0,
				3.f);

			DrawDebugLine(GetWorld(),
				CenterMuzzle->GetComponentLocation(), InterceptionPoint, FColor::Purple, false, 3.0f, 0, 5.f);

			// TODO: Debug the point of the target when the projectile needs to be fired, basically last possible fire moment
		}
		
		return true;
	}
	return false;
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
	return Distance <= Range;
}




void ATurret::Fire() const
{
	// Spawn Parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn the turret projectile
	const AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, CenterMuzzle->GetComponentLocation(), CenterMuzzle->GetComponentRotation(), SpawnParams);
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
	float NewPitch = FMath::FInterpTo(CurrentRotation.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), TurnSpeed);
	NewPitch = FMath::Clamp(NewPitch, -70.0f, 80.0f);
	RotationPoint->SetWorldRotation(FRotator(NewPitch, CurrentRotation.Yaw, CurrentRotation.Roll));
}


FRotator ATurret::GetIdleRotation() const
{
	FVector Direction = GetActorForwardVector();
	FRotator ForwardRot = FRotationMatrix::MakeFromX(Direction).Rotator();
	ForwardRot.Pitch += 25.f;
	return ForwardRot;
}
