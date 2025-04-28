// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;

#include "BaseProjectile.generated.h"


UCLASS()
class AIRCRAFTSIMULATOR_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UPrimitiveComponent* CollisionComponent;

	/** is it available in pool*/
	bool bIsAvailable;

	//Called when weapons are used
	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector LaunchLocation,FVector LaunchDirection, float OwnersCurrentSpeed);

	//Called when projectile is distroyed or needs to be rest
	UFUNCTION(BlueprintCallable)
	void ResetProjectilToPool();

	virtual float GetProjectileStartSpeed(float OwnerCurrentSpeed) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Movement");
	float ProjectileMaxSpeed = 10000;

	/**How fast the projectile accelerated from current speed to max speed*/
	//Wont be used for bullet because its initial speed will be equal to max speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Movement")
	float ProjectileAcceleration = 5000;


private:

	FTimerHandle ResetTimerHandle;

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

};
