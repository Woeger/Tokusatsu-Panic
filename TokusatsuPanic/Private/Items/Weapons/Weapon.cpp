// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"

AWeapon::AWeapon()
{
	WeaponHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Hitbox"));
	WeaponHitbox->SetupAttachment(GetRootComponent());
	WeaponHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, APawn* InInstigator)
{
	AttachMeshToSocket(InParent, InSocketName);
	SetInstigator(InInstigator);
	ItemState = EItemState::EIS_Equipped;
}

//Equipping

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

//Overlapping

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

//Tracing

void AWeapon::BoxTrace(FHitResult& Hit)
{
	const FVector StartTrace = StartHitTrace->GetComponentLocation();
	const FVector EndTrace = EndHitTrace->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, StartTrace, EndTrace, BoxTraceExtent, StartHitTrace->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
		false, ActorsToIgnore, bShowDebugBox ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, Hit, true);

	IgnoreActors.AddUnique(Hit.GetActor());
}

void AWeapon::ExecuteGetHit(FHitResult& Hit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(Hit.GetActor());

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(Hit.GetActor(), Hit.ImpactPoint);
	}
}

void AWeapon::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{
	FHitResult Hit;
	BoxTrace(Hit);

	if (Hit.GetActor())
	{
		UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(Hit);
		CreateFields(Hit.ImpactPoint);
	}
}
