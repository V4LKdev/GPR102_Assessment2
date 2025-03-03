// CG Spectrum 2025

// This file's header
#include "Turret.h"

#include "TurretProjectile.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Timer to update rotation every few seconds
	static float TestYaw = 0.0f;
	static float TestPitch = 0.0f;
	static float TimeSinceLastRotation = 0.0f;
	TimeSinceLastRotation += DeltaTime;

	if (TimeSinceLastRotation > 2.0f)  // Change target every 2 seconds
	{
		TimeSinceLastRotation = 0.0f;

		// Generate random test yaw and pitch
		TestYaw = FMath::RandRange(-90.0f, 90.0f);
		TestPitch = FMath::RandRange(-45.0f, 45.0f);

		// Set new rotation targets
		Fire();
	}

	SetYaw(TestYaw);
	SetPitch(TestPitch);
}

void ATurret::Fire() const
{
	// Spawn Parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn the turret projectile
	AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, CentreMuzzle->GetComponentLocation(), CentreMuzzle->GetComponentRotation(), SpawnParams);
	if (SpawnedProjectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Turret Projectile spawned."));
	}
}

void ATurret::SetYaw(float TargetYaw) const
{
	FRotator CurrentRotation = RotationPoint->GetComponentRotation();
	float NewYaw = FMath::FInterpTo(CurrentRotation.Yaw, TargetYaw, GetWorld()->GetDeltaSeconds(), TurnSpeed);
	RotationPoint->SetWorldRotation(FRotator(CurrentRotation.Pitch, NewYaw, CurrentRotation.Roll));
}

void ATurret::SetPitch(float TargetPitch) const
{
	FRotator CurrentRotation = RotationPoint->GetComponentRotation();
	float NewPitch = FMath::FInterpTo(CurrentRotation.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), TurnSpeed);
	RotationPoint->SetWorldRotation(FRotator(NewPitch, CurrentRotation.Yaw, CurrentRotation.Roll));
}

