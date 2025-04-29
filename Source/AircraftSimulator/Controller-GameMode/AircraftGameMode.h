// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AircraftSimulator//Interfaces/ProjectilePoolInterface.h"

class ABulletProjectilePool;
class AMissileProjectilePool;

#include "AircraftGameMode.generated.h"

/**
 * 
 */

UCLASS()
class AIRCRAFTSIMULATOR_API AAircraftGameMode : public AGameMode, public IProjectilePoolInterface
{
	GENERATED_BODY()


public:

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, Category = "Pool")
	TSubclassOf<ABulletProjectilePool> BulletPoolClass;

	UPROPERTY(EditAnywhere, Category = "Pool")
	TSubclassOf<AMissileProjectilePool> MissilePoolClass;

	virtual class AMissileProjectilePool* GetMissilePool_Implementation() const ;
	virtual class ABulletProjectilePool* GetBulletPool_Implementation() const ;
	
private:
	
	AMissileProjectilePool* MissilePoolInstance;
	ABulletProjectilePool* BulletPoolInstance;
};
