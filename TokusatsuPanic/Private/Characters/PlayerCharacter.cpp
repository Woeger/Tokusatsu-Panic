// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraBoom);

	jumping = false;
	jumpCount = 0;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::MoveForward(float Value)
{

	if ((Controller != nullptr) && (Value != 0.f))
	{
		//Find which way is forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//Move in that direction
		AddMovementInput(Direction, Value);
	}

}

void APlayerCharacter::Strafe(float Value)
{

	if ((Controller != nullptr) && (Value != 0.f))
	{
		//Find which way is right
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//Move in that direction
		AddMovementInput(Direction, Value);
	}

}

void APlayerCharacter::TurnYaw(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::TurnPitch(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::CheckJump()
{
	if (jumping)
	{
		jumping = false;
	}

	else
	{
		jumping = true;
		jumpCount++;

		if (jumpCount == 2)
		{
			LaunchCharacter(FVector(0, 0, 500), false, true);
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (jumping)
	{
		Jump();
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Strafe"), this, &APlayerCharacter::Strafe);
	PlayerInputComponent->BindAxis(TEXT("TurnYaw"), this, &APlayerCharacter::TurnYaw);
	PlayerInputComponent->BindAxis(TEXT("TurnPitch"), this, &APlayerCharacter::TurnPitch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::CheckJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::CheckJump);

}

