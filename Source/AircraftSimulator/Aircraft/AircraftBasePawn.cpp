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

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(StaticMesh);
	SpringArm->TargetOffset.Z = 80.f;
	
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
	FRotator NewRotation = GetActorRotation();
	NewRotation.Roll += Value.Get<float>();
	SetActorRotation(NewRotation);

	UE_LOG(LogTemp, Warning, TEXT("%f"), Value.Get<float>());
}

void AAircraftBasePawn::PitchInput(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Value.Get<float>());
	FRotator NewRotation = GetActorRotation();
	NewRotation.Pitch += Value.Get<float>();
	SetActorRotation(NewRotation);
}

void AAircraftBasePawn::YawInput(const FInputActionValue& Value)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += Value.Get<float>();
	SetActorRotation(NewRotation);
}
