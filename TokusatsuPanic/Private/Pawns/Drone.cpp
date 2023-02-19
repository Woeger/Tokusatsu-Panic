// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Drone.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ADrone::ADrone()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);
	SetRootComponent(Capsule);

	DroneMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DroneMesh"));
	DroneMesh->SetupAttachment(Capsule);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm")); 
	SpringArm->SetupAttachment(Capsule);
	SpringArm->TargetArmLength = 300.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArm);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ADrone::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Movement
void ADrone::MoveForward(float Value)
{

	if ((Controller != nullptr) && (Value != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
	}

}

void ADrone::TurnYaw(float Value)
{
	AddControllerYawInput(Value);
}

void ADrone::TurnPitch(float Value)
{
	AddControllerPitchInput(Value);
}

// Called to bind functionality to input
void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ADrone::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("TurnYaw"), this, &ADrone::TurnYaw);
	PlayerInputComponent->BindAxis(TEXT("TurnPitch"), this, &ADrone::TurnPitch);

}

