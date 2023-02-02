// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;

UENUM(BlueprintType)
enum class EEquippedState : uint8
{
	EES_Unequipped UMETA(DisplayName = "Unequipped"),
	EES_Equipped1H UMETA(DisplayName = "Equipped One-Handed"),
	EES_Equipped2H UMETA(DisplayName = "Equipped Two-Handed")
};

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

	void MoveForward(float Value);
	void Strafe(float Value);
	void TurnYaw(float Value);
	void TurnPitch(float Value);
	void EKeyPress();
	void Attack();

private:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY()
	bool jumping;

	UPROPERTY()
	int jumpCount;

	void CheckJump();

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	EEquippedState EquipState = EEquippedState::EES_Unequipped;

	//Animation montages
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;
};
