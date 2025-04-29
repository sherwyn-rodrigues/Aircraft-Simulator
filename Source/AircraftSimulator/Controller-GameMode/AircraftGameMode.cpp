// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Controller-GameMode/AircraftGameMode.h"
#include "AircraftSimulator/Projectiles/ProjectilePool/BulletProjectilePool.h"
#include "AircraftSimulator/Projectiles/ProjectilePool/MissileProjectilePool.h"

void AAircraftGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (MissilePoolClass)
	{
		MissilePoolInstance = GetWorld()->SpawnActor<AMissileProjectilePool>(MissilePoolClass, FVector::ZeroVector, FRotator::ZeroRotator);
	}

	if (BulletPoolClass)
	{
		BulletPoolInstance = GetWorld()->SpawnActor<ABulletProjectilePool>(BulletPoolClass, FVector::ZeroVector, FRotator::ZeroRotator);
	}

}

AMissileProjectilePool* AAircraftGameMode::GetMissilePool_Implementation() const
{
	return MissilePoolInstance;
}

ABulletProjectilePool* AAircraftGameMode::GetBulletPool_Implementation() const
{
	return BulletPoolInstance;
}
