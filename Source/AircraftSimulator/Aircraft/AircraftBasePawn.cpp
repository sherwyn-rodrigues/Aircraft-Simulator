// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftSimulator/Aircraft/AircraftBasePawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	Super::BeginPlay();
	CurrentThrust = MinThrustSpeed;
	CurrentSpeed = MinThrustSpeed;
}

// Called every frame
void AAircraftBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePosition(DeltaTime);

}

// Called to bind functionality to input
void AAircraftBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAircraftBasePawn::UpdatePosition(float DeltaTime)
{
	NewPosition = (CurrentSpeed * DeltaTime) * GetActorForwardVector();
	AddActorWorldOffset(NewPosition);

	AppliedGravity = FMath::GetMappedRangeValueClamped(
		FVector2D(0, MaxThrustNotToFallSpeed),
		FVector2D(Gravity, 0),
		CurrentSpeed
	);
}


