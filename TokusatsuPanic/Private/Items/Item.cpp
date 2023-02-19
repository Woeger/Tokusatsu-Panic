// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "TokusatsuPanic/DebugMacro.h"
#include "Components/SphereComponent.h"
#include "Characters/PlayerCharacter.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::CollisionSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::CollisionSphereOverlapEnd);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Floating)
	{
		const float Z = TransformedSin();
		AddActorWorldOffset(FVector(0.f, 0.f, Z));
	}
}

//Floating/Trig behaviour

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

//Overlaps
void AItem::CollisionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->SetOverlappingItem(this);
	}
}

void AItem::CollisionSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->SetOverlappingItem(nullptr);
	}
}

