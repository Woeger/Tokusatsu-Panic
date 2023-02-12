// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "CharacterEnums.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;

UCLASS()
class TOKUSATSUPANIC_API APlayerCharacter : public ACharacterBase
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

	//Attacking
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;

	//Equipping
	bool CanUnarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Unarm();
	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquip();

	//Montage Functions
	void PlayEquipMontage(FName SectionName);

private:

	//Camera
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
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Idle;

	//Animation montages
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;
};
