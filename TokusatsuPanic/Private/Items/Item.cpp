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

	FVector Location = GetActorLocation();
	UWorld* World = GetWorld();
	FVector Forward = GetActorForwardVector();

	DRAW_SPHERE(Location);
	DRAW_VECTOR(Location, Location + Forward * 100.f);

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

