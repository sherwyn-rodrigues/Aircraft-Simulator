// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Projectiles/BulletBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ABulletBase::ABulletBase()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
}
