// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
class ABulletProjectilePool;
class AMissileProjectilePool;
#include "AircraftGameMode.generated.h"

/**
 * 
 */

UCLASS()
class AIRCRAFTSIMULATOR_API AAircraftGameMode : public AGameMode
{
	GENERATED_BODY()


public:

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, Category = "Pool")
	TSubclassOf<ABulletProjectilePool> BulletPoolClass;

	UPROPERTY(EditAnywhere, Category = "Pool")
	TSubclassOf<AMissileProjectilePool> MissilePoolClass;
	
private:
	
	ABulletProjectilePool* BulletPoolInstance;
	AMissileProjectilePool* MissilePoolInstance;
};
