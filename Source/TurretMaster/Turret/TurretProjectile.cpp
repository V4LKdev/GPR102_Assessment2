﻿// CG Spectrum, Nic 2025

// This file's header
#include "TurretProjectile.h"

// Other includes
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ATurretProjectile::ATurretProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(BaseMesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void ATurretProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATurretProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

