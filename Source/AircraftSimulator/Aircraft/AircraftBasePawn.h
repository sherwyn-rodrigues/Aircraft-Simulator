// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AircraftBasePawn.generated.h"

UCLASS()
class AIRCRAFTSIMULATOR_API AAircraftBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAircraftBasePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float CalculateApplyGravity();

	float CalculateCurrentSpeed(float DeltaTime);

	void PrintStats();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArm;

	void UpdatePosition(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Constants")
	float MinThrustNotToFallSpeed = 4000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Constants")
	float MaxThrust = 10000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Constants")
	float Gravity = 9800;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Constants")
	float Drag = 0.25;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CurrentThrust = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CurrentSpeed = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector NewPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AppliedGravity = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FHitResult OutHit;
};
