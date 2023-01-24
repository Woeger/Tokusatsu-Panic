// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "TokusatsuPanic/DebugMacro.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Movement rate in cm/s
	float MovementRate = 50.f;
	float RotationRate = 45.f;

	//Movement rate * DeltaTime gives cm/frame
	AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));
	AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
	DRAW_SPHERE_SINGLEFRAME(GetActorLocation());
	DRAW_VECTOR_SINGLEFRAME(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f);
}

