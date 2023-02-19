// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOKUSATSUPANIC_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//Getters + Setters
public:
	//Health
	float GetHealthPercent();
	void TakeDamage(float Damage);
	//Stamina
	void UseStamina(float StaminaUsed);
	void RegenStamina(float DeltaTime);
	float GetStaminaPercent();
	FORCEINLINE float GetStamina() const { return CurrentStamina; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }

	bool IsAlive();

protected:
	virtual void BeginPlay() override;

private:
	//Health
	UPROPERTY(EditAnywhere, Category = "Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float CurrentHealth;

	//Stamina
	UPROPERTY(EditAnywhere, Category = "Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float CurrentStamina;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float StaminaRefillRate;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float DodgeCost;
};
