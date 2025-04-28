// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Projectiles/ProjectilePool/BaseProjectilePool.h"
#include "AircraftSimulator/Projectiles/BaseProjectile.h"

// Sets default values
ABaseProjectilePool::ABaseProjectilePool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABaseProjectilePool::BeginPlay()
{
	Super::BeginPlay();
	if (ProjectileClass)
	{
		for (int i = 0; i < PoolSize; i++)
		{
			ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, FVector(0, 0, -1000), FRotator::ZeroRotator);
			if (Projectile)
			{
				Projectile->ResetProjectilToPool();
				ProjectilePool.Add(Projectile);
			}
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Projectile cass not set for pool"));
}

ABaseProjectile* ABaseProjectilePool::GetAvailableProjectile()
{
	for (ABaseProjectile* Projectile : ProjectilePool)
	{
		if (Projectile && Projectile->bIsAvailable)
			return Projectile;
	}
	return nullptr;
}

