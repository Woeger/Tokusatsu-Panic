// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
	WeaponHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Hitbox"));
	WeaponHitbox->SetupAttachment(GetRootComponent());
	WeaponHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponHitbox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponHitbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	StartHitTrace = CreateDefaultSubobject<USceneComponent>(TEXT("Hitbox Trace Start"));
	StartHitTrace->SetupAttachment(GetRootComponent());

	EndHitTrace = CreateDefaultSubobject<USceneComponent>(TEXT("Hitbox Trace End"));
	EndHitTrace->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	WeaponHitbox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnHitboxOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	AttachMeshToSocket(InParent, InSocketName);
	ItemState = EItemState::EIS_Equipped;
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::CollisionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	Super::CollisionSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 25.f, FColor::Cyan, TEXT("Press 'E' to equip!"));
		}
	}
}

void AWeapon::CollisionSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::CollisionSphereOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{
	const FVector StartTrace = StartHitTrace->GetComponentLocation();
	const FVector EndTrace = EndHitTrace->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;

	FHitResult Hit;

	UKismetSystemLibrary::BoxTraceSingle(this, StartTrace, EndTrace, FVector(5.f, 5.f, 5.f), StartHitTrace->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
		false, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true);
}
