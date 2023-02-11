// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "CharacterBase.generated.h"

class AWeapon;
class UAnimMontage;
class UAttributeComponent;

UCLASS()
class TOKUSATSUPANIC_API ACharacterBase : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

protected:
	virtual void BeginPlay() override;

	//Attacking
	virtual bool CanAttack();
	virtual void Attack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UPROPERTY(VisibleAnywhere)
	AWeapon* EquippedWeapon;

	//Damage
	virtual void Death();
	virtual void GetHit(const FVector& Impact) override;
	void DirectionalHitReact(const FVector& Impact);

	//Montages
	virtual void PlayAttackMontage();
	void PlayHitReactMontage(FName SectionName);

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	//Components
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;
};
