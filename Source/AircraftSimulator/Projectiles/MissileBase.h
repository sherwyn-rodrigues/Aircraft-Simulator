// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AircraftSimulator/Projectiles/BaseProjectile.h"
class UCapsuleComponent;
#include "MissileBase.generated.h"

/**
 * 
 */
UCLASS()
class AIRCRAFTSIMULATOR_API AMissileBase : public ABaseProjectile
{
	GENERATED_BODY()


protected:

	virtual float GetProjectileStartSpeed(float OwnerCurrentSpeed) override;


public:

	AMissileBase();

	virtual void Tick(float DeltaTime) override;

private:
	float CurrentSpeed = 0;

};
