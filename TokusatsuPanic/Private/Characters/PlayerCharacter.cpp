// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

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

	Tags.Add(FName("PlayerCharacter"));
}

//Forward/Back movement
void APlayerCharacter::MoveForward(float Value)
{

	if ((Controller != nullptr) && (Value != 0.f) && ActionState == EActionState::EAS_Idle)
	{
		//Find which way is forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//Move in that direction
		AddMovementInput(Direction, Value);
	}

}

//Left/Right movement
void APlayerCharacter::Strafe(float Value)
{

	if ((Controller != nullptr) && (Value != 0.f) && ActionState == EActionState::EAS_Idle)
	{
		//Find which way is right
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//Move in that direction
		AddMovementInput(Direction, Value);
	}

}

//Turning
void APlayerCharacter::TurnYaw(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::TurnPitch(float Value)
{
	AddControllerPitchInput(Value);
}

//Equipping Items
void APlayerCharacter::EKeyPress()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this);
		EquipState = EEquippedState::EES_Equipped1H;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}

	else
	{
		if (CanUnarm())
		{
			PlayEquipMontage(FName("Unequip"));
			EquipState = EEquippedState::EES_Unequipped;
			ActionState = EActionState::EAS_Equipping;
		}

		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			EquipState = EEquippedState::EES_Equipped1H;
			ActionState = EActionState::EAS_Equipping;
		}
	}
}

bool APlayerCharacter::CanUnarm()
{
	return ActionState == EActionState::EAS_Idle && EquipState != EEquippedState::EES_Unequipped;
}

void APlayerCharacter::Unarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

bool APlayerCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Idle && EquipState == EEquippedState::EES_Unequipped && EquippedWeapon;
}

void APlayerCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void APlayerCharacter::FinishEquip()
{
	ActionState = EActionState::EAS_Idle;
}


//Attacking
void APlayerCharacter::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attack;
	}
}

void APlayerCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Idle;
}

bool APlayerCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Idle && EquipState != EEquippedState::EES_Unequipped;
}

void APlayerCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponHitbox())
	{
		EquippedWeapon->GetWeaponHitbox()->SetCollisionEnabled(CollisionEnabled);
	}

	EquippedWeapon->IgnoreActors.Empty();
}

//Montages

void APlayerCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
	}

	const int32 RandomAttack = FMath::RandRange(1, 3);
	FName SectionName = FName();

	switch (RandomAttack)
	{
	case 1:
		SectionName = FName("Attack1");
		break;
	case 2:
		SectionName = FName("Attack2");
		break;
	case 3:
		SectionName = FName("Attack3");
		break;
	}

	AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
}

void APlayerCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}

}

//Double jump
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

	//Movement Bindings
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Strafe"), this, &APlayerCharacter::Strafe);
	PlayerInputComponent->BindAxis(TEXT("TurnYaw"), this, &APlayerCharacter::TurnYaw);
	PlayerInputComponent->BindAxis(TEXT("TurnPitch"), this, &APlayerCharacter::TurnPitch);

	//Jump Bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::CheckJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::CheckJump);

	//Equip Bindings
	PlayerInputComponent->BindAction("Equip", IE_Released, this, &APlayerCharacter::EKeyPress);

	//Attack Bindings
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &APlayerCharacter::Attack);
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	jumpCount = 0;
}

