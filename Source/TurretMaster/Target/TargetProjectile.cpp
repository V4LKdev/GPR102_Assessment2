// CG Spectrum 2025

#include "TargetProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TargetProjectile)

ATargetProjectile::ATargetProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(BaseMesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootComponent);
}

FTargetData ATargetProjectile::GetTargetData_Implementation() const
{
	FTargetData Data;

	Data.Location = GetActorLocation();
	Data.Velocity = GetVelocity();

	return Data;
}

void ATargetProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (bDrawDebug)
	{
		PredictAndVisualizeProjectilePath();
	}
}

void ATargetProjectile::PredictAndVisualizeProjectilePath() const
{
	if (!ProjectileMovement)
	{
		return;
	}

	// If the projectile doesn't have gravity, just draw a line
	if (ProjectileMovement->ProjectileGravityScale == 0.f)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), ProjectileMovement->Velocity * 1000.f, FColor::Green, false, 3.f);
		return;
	}

	// Set up the prediction parameters
	FPredictProjectilePathParams Params;
	Params.StartLocation = GetActorLocation();
	Params.LaunchVelocity = ProjectileMovement->Velocity;
	Params.bTraceComplex = true;
	Params.bTraceWithCollision = true;
	Params.ProjectileRadius = 10.f;
	Params.MaxSimTime = 3.f;
	Params.SimFrequency = 30;
	Params.DrawDebugType = EDrawDebugTrace::ForDuration;
	Params.DrawDebugTime = 3.f;

	FPredictProjectilePathResult Result;

	UGameplayStatics::PredictProjectilePath(GetWorld(), Params, Result);
	
}

void ATargetProjectile::Death()
{
	OnTargetDestroyed.Broadcast(this);
}

void ATargetProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

