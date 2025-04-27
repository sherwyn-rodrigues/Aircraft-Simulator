// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AircraftSimulator/Projectiles/BaseProjectile.h"
class USphereComponent;
#include "BulletBase.generated.h"

/**
 * 
 */
UCLASS()
class AIRCRAFTSIMULATOR_API ABulletBase : public ABaseProjectile
{
	GENERATED_BODY()

public:
	ABulletBase();
};
