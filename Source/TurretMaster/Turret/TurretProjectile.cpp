// CG Spectrum, Nic 2025

// This file's header
#include "TurretProjectile.h"

// Other includes
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ATurretProjectile::ATurretProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(BaseMesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = BaseMesh;
}

void ATurretProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATurretProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

