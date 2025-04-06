// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Projectiles/MissileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"

AMissileBase::AMissileBase()
{
	//initialise capsule component as set as root
	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollider"));
	RootComponent = CapsuleCollider;
	StaticMesh->SetupAttachment(CapsuleCollider);

	//Missile starts slow and then accelerates
	ProjectileMovement->InitialSpeed = 0;
	ProjectileMovement->MaxSpeed = 10000;

}