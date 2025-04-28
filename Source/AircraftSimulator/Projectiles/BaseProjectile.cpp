// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Projectiles/BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	bIsAvailable = true;

}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseProjectile::LaunchProjectile(FVector LaunchLocation, FVector LaunchDirection, float OwnersCurrentSpeed)
{
	SetActorLocation(LaunchLocation);
	SetActorRotation(LaunchDirection.Rotation());

	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);

	ProjectileMovement->Velocity = LaunchDirection.GetSafeNormal() * GetProjectileStartSpeed(OwnersCurrentSpeed);
	ProjectileMovement->InitialSpeed = ProjectileMovement->Velocity.Size();
	ProjectileMovement->MaxSpeed = ProjectileMaxSpeed;
	//ProjectileMovement->accele
}

void ABaseProjectile::ResetProjectilToPool()
{
}

float ABaseProjectile::GetProjectileStartSpeed(float OwnerCurrentSpeed) const
{
	//By default speed from current actor speed to projectile max speed
	// change this to return MaxProjectileSpeed in bullet base class
	return OwnerCurrentSpeed;
}

void ABaseProjectile::OnComponentOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		ResetProjectilToPool();
	}
}

