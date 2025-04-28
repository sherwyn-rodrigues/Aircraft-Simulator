// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectilePool.generated.h"

UCLASS()
class AIRCRAFTSIMULATOR_API ABaseProjectilePool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectilePool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	class ABaseProjectile* GetAvailableProjectile();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Pool")
	TSubclassOf<class ABaseProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Pool")
	int PoolSize = 20;

	UPROPERTY()
	TArray<ABaseProjectile*> ProjectilePool;

};
