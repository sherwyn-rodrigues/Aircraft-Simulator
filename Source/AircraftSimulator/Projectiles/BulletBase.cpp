// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Projectiles/BulletBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ABulletBase::ABulletBase()
{
	//initialise sphere component as set as root
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = SphereCollider;
	StaticMesh->SetupAttachment(SphereCollider);

	//Bullet directly starts fast
	ProjectileMovement->InitialSpeed = 10000; //tweak values
	ProjectileMovement->MaxSpeed = 10000;
}
