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
}

// Called every frame
void AAircraftBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePosition(DeltaTime);
	PrintStats();

}

// Called to bind functionality to input
void AAircraftBasePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Triggered, this, &AAircraftBasePawn::YawInput);

		EnhancedInputComponent->BindAction(PitchAction, ETriggerEvent::Triggered, this, &AAircraftBasePawn::PitchInput);

		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AAircraftBasePawn::RollInput);

		EnhancedInputComponent->BindAction(LookAxisX, ETriggerEvent::Triggered, this, &AAircraftBasePawn::LookAroundYaw);

		EnhancedInputComponent->BindAction(LookAxisY, ETriggerEvent::Triggered, this, &AAircraftBasePawn::LookAroundPitch);
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
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Player Health: %.2f"), AppliedGravity));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Player Health: %.2f"), CurrentThrust));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Player Health: %.2f"), CurrentSpeed));
}

void AAircraftBasePawn::ThrottleInput(const FInputActionValue& Value)
{
}

void AAircraftBasePawn::RollInput(const FInputActionValue& Value)
{
	FRotator NewRotation;
	NewRotation.Roll += Value.Get<float>();
	//SetActorRotation(NewRotation);
	AddActorLocalRotation(NewRotation);

	UE_LOG(LogTemp, Warning, TEXT("%f"), Value.Get<float>());
}

void AAircraftBasePawn::PitchInput(const FInputActionValue& Value)
{
	FRotator NewRotation;
	NewRotation.Pitch += Value.Get<float>();
	//SetActorRotation(NewRotation);
	AddActorLocalRotation(NewRotation);
}

void AAircraftBasePawn::YawInput(const FInputActionValue& Value)
{
	FRotator NewRotation;
	NewRotation.Yaw += Value.Get<float>();
	//SetActorRotation(NewRotation);
	AddActorLocalRotation(NewRotation);
}

void AAircraftBasePawn::LookAroundPitch(const FInputActionValue& Value)
{
	FRotator NewRotation;
	NewRotation.Pitch += Value.Get<float>();
	SpringArm->AddLocalRotation(NewRotation);
	FRotator PlayerWorldRotation = SpringArm->GetRelativeRotation();
	PlayerWorldRotation.Roll = 0;
	SpringArm->SetRelativeRotation(PlayerWorldRotation);
}

void AAircraftBasePawn::LookAroundYaw(const FInputActionValue& Value)
{
	FRotator NewRotation;
	NewRotation.Yaw += Value.Get<float>();
	SpringArm->AddLocalRotation(NewRotation);
	FRotator PlayerWorldRotation = SpringArm->GetRelativeRotation();
	PlayerWorldRotation.Roll = 0;
	SpringArm->SetRelativeRotation(PlayerWorldRotation);
}

void AAircraftBasePawn::ResetCameraAngle()
{
	FRotator CurrentRelativeRotation = SpringArm->GetRelativeRotation();
	FRotator TargetLocalRotation = FRotator(0, 0, 0);
	//FMath::FInterpTo();
}
