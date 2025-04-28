// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Aircraft/AircraftBasePawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
AAircraftBasePawn::AAircraftBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SkeletalMesh);
	SpringArm->TargetOffset.Z = 200.f;
	SpringArm->TargetArmLength = 800;
	SpringArm->SetRelativeLocation(FVector(0,0,280));
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AAircraftBasePawn::BeginPlay()
{
	CurrentThrust = MinThrustNotToFallSpeed;
	CurrentSpeed = MinThrustNotToFallSpeed;
	Super::BeginPlay();
	//SetYawAndPitchLimits();
}

// Called every frame
void AAircraftBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePosition(DeltaTime);
	PrintStats();
	UpdateSmoothedRotation(DeltaTime);

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

		//Look Input
		EnhancedInputComponent->BindAction(LookAxisX, ETriggerEvent::Triggered, this, &AAircraftBasePawn::LookAroundYaw);
		EnhancedInputComponent->BindAction(LookAxisX, ETriggerEvent::Completed, this, &AAircraftBasePawn::TriggerResetCameraAngle);
		EnhancedInputComponent->BindAction(LookAxisY, ETriggerEvent::Triggered, this, &AAircraftBasePawn::LookAroundPitch);
		EnhancedInputComponent->BindAction(LookAxisY, ETriggerEvent::Completed, this, &AAircraftBasePawn::TriggerResetCameraAngle);

		//weapons Input
		EnhancedInputComponent->BindAction(FireMissilesInput, ETriggerEvent::Started, this, &AAircraftBasePawn::FireMissiles);
		EnhancedInputComponent->BindAction(FireMachingGunInput, ETriggerEvent::Started, this, &AAircraftBasePawn::FireMachineGun);
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
	if (CurrentSpeed < CurrentThrust)
	{
		return CurrentThrust;
	}
	else
	{
		return FMath::FInterpTo(CurrentSpeed, CurrentThrust, DeltaTime, Drag);
	}
}

void AAircraftBasePawn::PrintStats()
{
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Gravity: %.2f"), AppliedGravity));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Thrust : %.2f"), CurrentThrust));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Current Speed Health: %.2f"), CurrentSpeed));
}

void AAircraftBasePawn::ThrottleInput(const FInputActionValue& Value)
{
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

void AAircraftBasePawn::FireMissiles()
{
	UE_LOG(LogTemp, Warning, TEXT("Missiles"));
	return;
}

void AAircraftBasePawn::FireMachineGun()
{
	UE_LOG(LogTemp, Warning, TEXT("MachineGun"));
	return;
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