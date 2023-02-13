// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PlayerOverlay.h"
#include "Components/ProgressBar.h"

//Setting progress bars
void UPlayerOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthBarProgress)
	{
		HealthBarProgress->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaBarProgress)
	{
		StaminaBarProgress->SetPercent(Percent);
	}
}
