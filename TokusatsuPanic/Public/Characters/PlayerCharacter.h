// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"
#include "CharacterEnums.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;

UCLASS()
class TOKUSATSUPANIC_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

//Section for getters/setters
public:
	//Setter for overlapping item
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EEquippedState GetEquipState() { return EquipState; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Input Callbacks
	void MoveForward(float Value);
	void Strafe(float Value);
	void TurnYaw(float Value);
	void TurnPitch(float Value);
	void EKeyPress();
	void Attack();

	//Montage Functions
	void PlayAttackMontage();

private:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	//Jumping
	UPROPERTY()
	bool jumping;

	UPROPERTY()
	int jumpCount;

	void CheckJump();

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	//Enums
	EEquippedState EquipState = EEquippedState::EES_Unequipped;
	EActionState ActionState = EActionState::EAS_Idle;

	//Animation montages
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;
};
