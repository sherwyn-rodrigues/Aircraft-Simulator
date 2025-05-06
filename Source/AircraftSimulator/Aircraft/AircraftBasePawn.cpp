// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Aircraft/AircraftBasePawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "AircraftSimulator/Interfaces/ProjectilePoolInterface.h"
#include "GameFramework/GameModeBase.h"
#include "AircraftSimulator/Projectiles/BaseProjectile.h"
#include "AircraftSimulator/Projectiles/ProjectilePool/MissileProjectilePool.h"
#include "AircraftSimulator/Projectiles/ProjectilePool/BulletProjectilePool.h"

// Sets default values
AAircraftBasePawn::AAircraftBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SkeletalMesh;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SkeletalMesh);
	SpringArm->TargetArmLength = 800;
	SpringArm->SetRelativeLocation(FVector(0,0,500));
	SpringArm->bUsePawnControlRotation = false;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	//Spawn points for maching gun and missile
	BulletsSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawnPoint"));
	BulletsSpawnPoint->SetupAttachment(SkeletalMesh);
	LeftMissileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftMissileSpawnPoint"));
	LeftMissileSpawnPoint->SetupAttachment(SkeletalMesh);
	RightMissileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightMissileSpawnPoint"));
	RightMissileSpawnPoint->SetupAttachment(SkeletalMesh);
}

// Called when the game starts or when spawned
void AAircraftBasePawn::BeginPlay()
{
	Super::BeginPlay();

	ThrottleLevel = MinThrustNotToFallSpeed / MaxThrust;
	ThrottleLevel = FMath::Clamp(ThrottleLevel, 0.f, 1.f);

	// Let HandleThrottleInput use this to compute thrust and speed correctly in Tick
	CurrentThrust = MinThrustNotToFallSpeed;
	CurrentSpeed = MinThrustNotToFallSpeed;

	AGameModeBase* GameModeRef = UGameplayStatics::GetGameMode(this);
	if (GameModeRef && GameModeRef->Implements<UProjectilePoolInterface>())
	{
		MissilePoolRef = IProjectilePoolInterface::Execute_GetMissilePool(GameModeRef);
		BulletPoolRef = IProjectilePoolInterface::Execute_GetBulletPool(GameModeRef);
	}
}

// Called every frame
void AAircraftBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleThrottleInput(DeltaTime);
	UpdatePosition(DeltaTime);
	UpdateSmoothedRotation(DeltaTime);

	//SpringArm->TargetOffset = GetActorUpVector() * 200;
	PrintStats();

}

// Called to bind functionality to input
void AAircraftBasePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//player movement input
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Triggered, this, &AAircraftBasePawn::YawInput);
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Completed, this, &AAircraftBasePawn::YawInputCompleted);

		EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Triggered, this, &AAircraftBasePawn::PitchInput);
		EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Completed, this, &AAircraftBasePawn::PitchInputCompleted);

		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AAircraftBasePawn::RollInput);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &AAircraftBasePawn::RollInputComplete);

		EnhancedInputComponent->BindAction(ThrottleUpAction, ETriggerEvent::Started, this, &AAircraftBasePawn::ThrottleUpPressed);
		EnhancedInputComponent->BindAction(ThrottleUpAction, ETriggerEvent::Completed, this, &AAircraftBasePawn::ThrottleUpReleased);

		EnhancedInputComponent->BindAction(ThrottleDownAction, ETriggerEvent::Started, this, &AAircraftBasePawn::ThrottleDownPressed);
		EnhancedInputComponent->BindAction(ThrottleDownAction, ETriggerEvent::Completed, this, &AAircraftBasePawn::ThrottleDownReleased);

		//Look Input
		EnhancedInputComponent->BindAction(LookAxisX, ETriggerEvent::Triggered, this, &AAircraftBasePawn::LookAroundYaw);
		EnhancedInputComponent->BindAction(LookAxisX, ETriggerEvent::Completed, this, &AAircraftBasePawn::TriggerResetCameraAngle);
		EnhancedInputComponent->BindAction(LookAxisY, ETriggerEvent::Triggered, this, &AAircraftBasePawn::LookAroundPitch);
		EnhancedInputComponent->BindAction(LookAxisY, ETriggerEvent::Completed, this, &AAircraftBasePawn::TriggerResetCameraAngle);

		//weapons Input
		EnhancedInputComponent->BindAction(FireMissilesInput, ETriggerEvent::Started, this, &AAircraftBasePawn::FireMissiles);
		EnhancedInputComponent->BindAction(FireMachingGunInput, ETriggerEvent::Started, this, &AAircraftBasePawn::FireMachineGun);
		EnhancedInputComponent->BindAction(FireMachingGunInput, ETriggerEvent::Completed, this, &AAircraftBasePawn::StopMachineGun);
		EnhancedInputComponent->BindAction(FireMachingGunInput, ETriggerEvent::Canceled, this, &AAircraftBasePawn::StopMachineGun);

	}
}

void AAircraftBasePawn::UpdatePosition(float DeltaTime)
{
	CurrentSpeed = CalculateCurrentSpeed(DeltaTime);
	AppliedGravity = CalculateApplyGravity();

	NewPosition = (CurrentSpeed * DeltaTime) * GetActorForwardVector();
	NewPosition = FVector(NewPosition.X, NewPosition.Y, NewPosition.Z - AppliedGravity);
	//movement
	AddActorWorldOffset(NewPosition, true, &OutHit);
}

float AAircraftBasePawn::CalculateApplyGravity()
{
	return FMath::GetMappedRangeValueClamped(
		FVector2D(0, MinThrustNotToFallSpeed),
		FVector2D(Gravity, 0),
		CurrentSpeed
	);
}

float AAircraftBasePawn::CalculateCurrentSpeed(float DeltaTime)
{
	return FMath::FInterpTo(CurrentSpeed, CurrentThrust, DeltaTime, Drag);
}

void AAircraftBasePawn::PrintStats()
{
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Gravity: %.2f"), AppliedGravity));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Thrust : %.2f"), CurrentThrust));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Current Speed Health: %.2f"), CurrentSpeed));
}

void AAircraftBasePawn::RollInput(const FInputActionValue& Value)
{
	TargetRollInput = Value.Get<float>();
}

void AAircraftBasePawn::PitchInput(const FInputActionValue& Value)
{
	TargetPitchInput = Value.Get<float>();
}

void AAircraftBasePawn::YawInput(const FInputActionValue& Value)
{
	TargetYawInput = Value.Get<float>();
}


void AAircraftBasePawn::LookAroundPitch(const FInputActionValue& Value)
{
	FRotator NewRotation;
	NewRotation.Pitch += Value.Get<float>();

	if (Value.Get<float>() > 0 && SpringArm->GetRelativeRotation().Pitch < CameraLookClamp)
	{
		SpringArm->AddLocalRotation(NewRotation);
		ResetRoll();
	}
	else if(Value.Get<float>() < 0 && SpringArm->GetRelativeRotation().Pitch > -CameraLookClamp)
	{
		SpringArm->AddLocalRotation(NewRotation);
		ResetRoll();
	}
}

void AAircraftBasePawn::LookAroundYaw(const FInputActionValue& Value)
{
	FRotator NewRotation;
	NewRotation.Yaw += Value.Get<float>();

	if (Value.Get<float>() > 0 && SpringArm->GetRelativeRotation().Yaw < CameraLookClamp)
	{
		SpringArm->AddLocalRotation(NewRotation);
		ResetRoll();
	}
	else if (Value.Get<float>() < 0 && SpringArm->GetRelativeRotation().Yaw > -CameraLookClamp)
	{
		SpringArm->AddLocalRotation(NewRotation);
		ResetRoll();
	}
}

void AAircraftBasePawn::ResetCameraAngle()
{
	FRotator CurrentRelativeRotation = SpringArm->GetRelativeRotation();
	FRotator TargetLocalRotation = FMath::RInterpTo(CurrentRelativeRotation, FRotator(0.f, 0.f, 0.f), GetWorld()->GetDeltaSeconds(), 1.0f);

	bool bNearlyEqualPitch = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocalRotation.Pitch, 0, 1);
	bool bNearlyEqualYaw = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocalRotation.Yaw, 0, 1);

	if (bNearlyEqualYaw && bNearlyEqualPitch)
	{
		SpringArm->SetRelativeRotation(FRotator(0,0,0));
		GetWorldTimerManager().ClearTimer(TriggerResetCameraHandle);
	}
	else 
	{
		SpringArm->SetRelativeRotation(TargetLocalRotation);
	}
}

void AAircraftBasePawn::ResetRoll()
{
	FRotator PlayerWorldRotation = SpringArm->GetRelativeRotation();
	PlayerWorldRotation.Roll = 0;
	SpringArm->SetRelativeRotation(PlayerWorldRotation);
}

void AAircraftBasePawn::TriggerResetCameraAngle()
{
	GetWorldTimerManager().SetTimer(TriggerResetCameraHandle, this, &AAircraftBasePawn::ResetCameraAngle, 0.01f, true, 0.5f);
}

//wont work for local rotations
void AAircraftBasePawn::SetYawAndPitchLimits()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
		{
			CameraManager->ViewYawMin = -CameraLookClamp;
			CameraManager->ViewYawMax = CameraLookClamp;
			CameraManager->ViewPitchMin = -CameraLookClamp;
			CameraManager->ViewPitchMax = CameraLookClamp;
		}
	}
}

void AAircraftBasePawn::UpdateSmoothedRotation(float DeltaTime)
{
	SmoothedPitchInput = FMath::FInterpTo(SmoothedPitchInput, TargetPitchInput, DeltaTime, InterpSpeed);
	SmoothedYawInput = FMath::FInterpTo(SmoothedYawInput, TargetYawInput, DeltaTime, InterpSpeed);
	SmoothedRollInput = FMath::FInterpTo(SmoothedRollInput, TargetRollInput, DeltaTime, InterpSpeed);

	float PitchDelta = SmoothedPitchInput * PitchRotationSpeed * DeltaTime;
	float YawDelta = SmoothedYawInput * YawRotationSpeed * DeltaTime;
	float RollDelta = SmoothedRollInput * RollRotationSpeed * DeltaTime;

	FRotator DeltaRotator(PitchDelta, YawDelta, RollDelta);
	AddActorLocalRotation(DeltaRotator);
}

void AAircraftBasePawn::FireMachineGun()
{
	GetWorldTimerManager().SetTimer(MachineGunTriggerHandle, this, &AAircraftBasePawn::ActivateMachineGun, BuklletFireInterval, true);
}

void AAircraftBasePawn::StopMachineGun()
{
	GetWorldTimerManager().ClearTimer(MachineGunTriggerHandle);
}

void AAircraftBasePawn::PitchInputCompleted()
{
	TargetPitchInput = 0;
}

void AAircraftBasePawn::YawInputCompleted()
{
	TargetYawInput = 0;
}

void AAircraftBasePawn::RollInputComplete()
{
	TargetRollInput = 0;
}

void AAircraftBasePawn::FireMissiles()
{
	//UE_LOG(LogTemp, Warning, TEXT("Missiles"));
	if (MissilePoolRef)
	{
		ABaseProjectile* MissileToLaunch = MissilePoolRef->GetAvailableProjectile();
		if (MissileToLaunch)
		{
			if (isLeftMissileSpawn && bIsLeftMissileSlotAvailable)
			{
				MissileToLaunch->LaunchProjectile(LeftMissileSpawnPoint->GetComponentLocation(), GetActorForwardVector(), CurrentSpeed);
				isLeftMissileSpawn = !isLeftMissileSpawn;
				bIsLeftMissileSlotAvailable = false;
				GetWorldTimerManager().SetTimer(LeftMissileSlotResetHandle, this, &AAircraftBasePawn::ResetLeftMissileSlot, MissileReloadTime, false);
			}
			else if(!isLeftMissileSpawn && bIsRightMissileSlotAvailable)
			{
				MissileToLaunch->LaunchProjectile(RightMissileSpawnPoint->GetComponentLocation(), GetActorForwardVector(), CurrentSpeed);
				isLeftMissileSpawn = !isLeftMissileSpawn;
				bIsRightMissileSlotAvailable = false;
				GetWorldTimerManager().SetTimer(RightMissileSlotResetHandle, this, &AAircraftBasePawn::ResetRightMissileSlot, MissileReloadTime, false);
			}
		}
	}
}

void AAircraftBasePawn::ResetRightMissileSlot()
{
	bIsRightMissileSlotAvailable = true;
	GetWorldTimerManager().ClearTimer(RightMissileSlotResetHandle);
}

void AAircraftBasePawn::ResetLeftMissileSlot()
{
	bIsLeftMissileSlotAvailable = true;
	GetWorldTimerManager().ClearTimer(LeftMissileSlotResetHandle);
}

void AAircraftBasePawn::ActivateMachineGun()
{
	if (BulletPoolRef)
	{
		ABaseProjectile* BulletToLaunch = BulletPoolRef->GetAvailableProjectile();
		BulletToLaunch->LaunchProjectile(BulletsSpawnPoint->GetComponentLocation(), GetActorForwardVector(), CurrentSpeed);
	}
}


void AAircraftBasePawn::ThrottleUpPressed()
{
	bThrottleUpHeld = true;
}

void AAircraftBasePawn::ThrottleUpReleased()
{
	bThrottleUpHeld = false;
}

void AAircraftBasePawn::ThrottleDownPressed()
{
	bThrottleDownHeld = true;
}

void AAircraftBasePawn::ThrottleDownReleased()
{
	bThrottleDownHeld = false;
}

void AAircraftBasePawn::HandleThrottleInput(float DeltaTime)
{
	const bool bNeutralInput = bThrottleUpHeld && bThrottleDownHeld;
	const float MinThrottleLevel = MinThrustNotToFallSpeed / MaxThrust;

	if (!bNeutralInput)
	{
		if (bThrottleUpHeld)
		{
			ThrottleLevel += AccelerationRate * DeltaTime;
		}
		else if (bThrottleDownHeld)
		{
			ThrottleLevel -= DecelerationRate * DeltaTime;
		}
		else
		{
			if (ThrottleLevel > MinThrottleLevel)
			{
				ThrottleLevel -= PassiveThrottleDecayRate * DeltaTime;
				if (ThrottleLevel < MinThrottleLevel)
				{
					ThrottleLevel = MinThrottleLevel;
				}
			}
			else if (ThrottleLevel < MinThrottleLevel)
			{
				ThrottleLevel = FMath::FInterpTo(ThrottleLevel, MinThrottleLevel, DeltaTime, 2.0f);
			}
		}
	}
	else
	{
		if (ThrottleLevel > MinThrottleLevel)
		{
			ThrottleLevel -= PassiveThrottleDecayRate * DeltaTime;
			if (ThrottleLevel < MinThrottleLevel)
			{
				ThrottleLevel = MinThrottleLevel;
			}
		}
		else if (ThrottleLevel < MinThrottleLevel)
		{
			ThrottleLevel = FMath::FInterpTo(ThrottleLevel, MinThrottleLevel, DeltaTime, 2.0f);
		}
	}

	ThrottleLevel = FMath::Clamp(ThrottleLevel, 0.f, 1.f);
	CurrentThrust = FMath::Lerp(0.f, MaxThrust, ThrottleLevel);
}
