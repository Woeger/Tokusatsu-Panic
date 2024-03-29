// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/PlayerHUD.h"
#include "HUD/PlayerOverlay.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"

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

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);


	jumping = false;
	jumpCount = 0;

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("PlayerCharacter"));

	//Get HUD
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	if (PlayerController)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());

		if (PlayerHUD)
		{
			PlayerOverlay = PlayerHUD->GetOverlay();

			if (PlayerOverlay && Attributes)
			{
				PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				PlayerOverlay->SetStaminaBarPercent(1.f);
			}
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && PlayerOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

	if (jumping)
	{
		Jump();
	}
}

//Movement
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
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquipState = EEquippedState::EES_Equipped1H;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}

	else
	{
		if (CanUnarm())
		{
			PlayMontageSection(EquipMontage, FName("Unequip"));
			EquipState = EEquippedState::EES_Unequipped;
			ActionState = EActionState::EAS_Equipping;
		}

		else if (CanArm())
		{
			PlayMontageSection(EquipMontage, FName("Equip"));
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

//Damage
void APlayerCharacter::GetHit_Implementation(const FVector& Impact, AActor* HitTaker)
{
	Super::GetHit_Implementation(Impact, HitTaker);
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_Hit;
	}

}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	return DamageAmount;
}

void APlayerCharacter::EndHit()
{
	ActionState = EActionState::EAS_Idle;
}

void APlayerCharacter::Death()
{
	Super::Death();
	ActionState = EActionState::EAS_Dead;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

//Dodging
void APlayerCharacter::Roll()
{
	if (CanRoll())
	{
		if (Attributes && PlayerOverlay)
		{
			Attributes->UseStamina(Attributes->GetDodgeCost());
			PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}

		PlayMontageSection(DodgeMontage, FName("Default"));
		ActionState = EActionState::EAS_Roll;
	}
}

bool APlayerCharacter::CanRoll()
{
	return ActionState == EActionState::EAS_Idle && Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

void APlayerCharacter::FinishRoll()
{
	ActionState = EActionState::EAS_Idle;
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

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	jumpCount = 0;
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

	//Dodge Bindings
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &APlayerCharacter::Roll);

	//Equip Bindings
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &APlayerCharacter::EKeyPress);

	//Attack Bindings
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
}

