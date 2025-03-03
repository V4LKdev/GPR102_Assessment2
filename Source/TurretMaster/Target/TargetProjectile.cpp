// CG Spectrum 2025

// This file's header
#include "TargetProjectile.h"

// Other includes
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ATargetProjectile::ATargetProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(BaseMesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

// Called when the game starts or when spawned
void ATargetProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

