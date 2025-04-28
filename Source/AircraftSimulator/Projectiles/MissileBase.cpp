// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Projectiles/MissileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"


AMissileBase::AMissileBase()
{
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollider"));
}

void AMissileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsAvailable && ProjectileMovement)
	{
		CurrentSpeed = FMath::Min(CurrentSpeed + (ProjectileAcceleration * DeltaTime), ProjectileMaxSpeed);
		ProjectileMovement->Velocity = CurrentSpeed * GetActorForwardVector();
	}
}


float AMissileBase::GetProjectileStartSpeed(float OwnerCurrentSpeed)
{
	CurrentSpeed = OwnerCurrentSpeed;
	return CurrentSpeed;
}