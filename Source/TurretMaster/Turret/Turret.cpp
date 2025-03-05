// CG Spectrum, Nic 2025

#include "Turret.h"

#include "TurretProjectile.h"
#include "Components/SphereComponent.h"
#include "TurretMaster/TurretMasterLogChannels.h"
#include "TurretMaster/Interface/Targetable.h"

ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
	
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

void ATurret::BeginPlay()
{
	Super::BeginPlay();

	TurretArea->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
}

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
		// If there are any active targets, enable tick to trigger targeting logic
		const bool bShouldTickActor = !ActiveTargets.IsEmpty();
		SetActorTickEnabled(bShouldTickActor);
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
		
		
		FTargetData TargetData = ITargetable::Execute_GetTargetData(OtherActor);
		
		
		UE_LOG(LogTurretMaster, Log, TEXT("Turret %s: Target (%s) Entered Effective Range"), *GetName(), *OtherActor->GetName());
		ActiveTargets.Add(OtherActor);
	}
}


void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO: Calculate impact point

	// TODO: Set yaw and pitch

	// TODO: Check muzzle is pointed at impact point

	// TODO: If it is, FIRE!
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

