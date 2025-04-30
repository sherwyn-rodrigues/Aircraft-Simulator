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

	// Input Actions 
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FireMissilesInput;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FireMachingGunInput;
	
	//Throttle Input
	UFUNCTION(BlueprintCallable)
	void ThrottleInput(const FInputActionValue& Value);

	//Roll Input
	UFUNCTION(BlueprintCallable)
	void RollInput(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void RollInputComplete();

	//Pitch Input
	UFUNCTION(BlueprintCallable)
	void PitchInput(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void PitchInputCompleted();

	//Yaw Input
	UFUNCTION(BlueprintCallable)
	void YawInput(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void YawInputCompleted();

	//Look Around Yaw and Pitch Input
	UFUNCTION(BlueprintCallable)
	void LookAroundYaw(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void LookAroundPitch(const FInputActionValue& Value);

	// Smoothens rotation to make it feel better and not robotic
	UFUNCTION(BlueprintCallable)
	void UpdateSmoothedRotation(float DeltaTime);

	//Weapons Input function
	UFUNCTION(BlueprintCallable)
	void FireMissiles();

	UFUNCTION(BlueprintCallable)
	void FireMachineGun();

	UFUNCTION(BlueprintCallable)
	void StopMachineGun();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Skeletal Mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh;
	
	// Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	//Spring Arm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	//Maching Gun Projectile SpawnPoint
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* BulletsSpawnPoint;

	//Missile Projectile Spawn Points Note: 2 Spawn Points
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* LeftMissileSpawnPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* RightMissileSpawnPoint;


	//function to run on tick to give continous movement to jet
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

	/**Reload time for each missile slot before it can be used again */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MissileReloadTime = 3.0f;
	
	/**Interval between each bullet fired*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BuklletFireInterval = 0.5f;

	UFUNCTION(BlueprintCallable)
	void ResetCameraAngle();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float CameraLookClamp = 60;

private:

	//Resets the Look Around Camera 
	UFUNCTION(BlueprintCallable)
	void ResetRoll();
	
	UFUNCTION(BlueprintCallable)
	void TriggerResetCameraAngle();

	// timer handle to reset projectile back to pool after launch 
	FTimerHandle TriggerResetCameraHandle;

	void SetYawAndPitchLimits();

	// New smooth controls

	float TargetPitchInput = 0.0f;
	float TargetYawInput = 0.0f;
	float TargetRollInput = 0.0f;
	float SmoothedPitchInput = 0.0f;
	float SmoothedYawInput = 0.0f;
	float SmoothedRollInput = 0.0f;

	// Used For SMoothening the Rotation
	UPROPERTY(EditAnywhere, Category = "Flight Control")
	float PitchRotationSpeed = 60.f;
	UPROPERTY(EditAnywhere, Category = "Flight Control")
	float YawRotationSpeed = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Flight Control")
	float RollRotationSpeed = 60.0f;


	UPROPERTY(EditAnywhere, Category = "Flight Control")
	float InterpSpeed = 5.f;

	bool isLeftMissileSpawn = true;

	class AMissileProjectilePool* MissilePoolRef;
	class ABulletProjectilePool* BulletPoolRef;

	//missile reload variables and functions 

	//Booleans for left and right slot resp 
	bool bIsLeftMissileSlotAvailable = true;
	bool bIsRightMissileSlotAvailable = true;

	//Timer Handles for left and right slot resp 
	FTimerHandle RightMissileSlotResetHandle;
	FTimerHandle LeftMissileSlotResetHandle;

	void ResetRightMissileSlot();
	void ResetLeftMissileSlot();

	// Missile reload End 

	// Timer Handle to manage machine gun start and end 
	FTimerHandle MachineGunTriggerHandle;

	void ActivateMachineGun();

};
