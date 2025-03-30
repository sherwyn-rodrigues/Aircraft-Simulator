// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AircraftBasePawn.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PitchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* YawAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAxisX;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAxisY;


	UFUNCTION(BlueprintCallable)
	void ThrottleInput(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void RollInput(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void PitchInput(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void YawInput(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void LookAroundYaw(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void LookAroundPitch(const FInputActionValue& Value);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

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
