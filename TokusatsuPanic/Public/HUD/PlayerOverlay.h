// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlay.generated.h"

/**
 * 
 */
UCLASS()
class TOKUSATSUPANIC_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	//Setters for Progress Bars
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);

private:
	//Progress Bars
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBarProgress;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBarProgress;

	
};
