// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterBase.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Characters/CharacterEnums.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Attacking

void ACharacterBase::Attack()
{
}

bool ACharacterBase::CanAttack()
{
	return false;
}

void ACharacterBase::AttackEnd()
{
}

void ACharacterBase::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponHitbox())
	{
		EquippedWeapon->GetWeaponHitbox()->SetCollisionEnabled(CollisionEnabled);
	}

	EquippedWeapon->IgnoreActors.Empty();
}

//Damage

void ACharacterBase::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->TakeDamage(DamageAmount);
	}
}

void ACharacterBase::Death()
{
	PlayDeathMontage();
}

void ACharacterBase::GetHit_Implementation(const FVector& Impact, AActor* HitTaker)
{
	if (Attributes && Attributes->IsAlive() && HitTaker)
	{
		DirectionalHitReact(HitTaker->GetTargetLocation());
	}

	else
	{
		Death();
	}
}

void ACharacterBase::DirectionalHitReact(const FVector& Impact)
{
	//Obtain vectors needed for dot product
	const FVector EnemyForward = GetActorForwardVector();
	//Creating vector for hit impact parallel to forward vector
	const FVector ImpactVector(Impact.X, Impact.Y, GetActorLocation().Z);
	//Ensure hit vector is normalised
	const FVector HitVector = (Impact - GetActorLocation()).GetSafeNormal();

	//Convert from cos(angle) to angle in degrees
	const double CosHitAngle = FVector::DotProduct(EnemyForward, HitVector);
	double HitAngle = FMath::Acos(CosHitAngle);
	HitAngle = FMath::RadiansToDegrees(HitAngle);

	//Obtaining cross product to figure out if hit is from the left or right
	const FVector HitCrossProduct = FVector::CrossProduct(EnemyForward, HitVector);
	//Adjust angle accordingly
	if (HitCrossProduct.Z < 0)
	{
		HitAngle *= -1.f;
	}

	//Default to back hit
	FName SectionName = "ReactBack";

	//Check if angle is from the front, change hit reaction accordingly
	if (HitAngle >= -45.f && HitAngle < 45.f)
	{
		SectionName = "ReactFront";
	}

	else if (HitAngle >= -135.f && HitAngle < -45.f)
	{
		SectionName = "ReactLeft";
	}

	else if (HitAngle >= 45.f && HitAngle < 135.f)
	{
		SectionName = "ReactRight";
	}

	PlayHitReactMontage(FName(SectionName));
}

//Montages

void ACharacterBase::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ACharacterBase::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 SectionSelection = FMath::RandRange(0, SectionNames.Num() - 1);
	PlayMontageSection(Montage, SectionNames[SectionSelection]);
	return SectionSelection;
}

int32 ACharacterBase::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ACharacterBase::PlayDeathMontage()
{
	int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);

	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}

void ACharacterBase::PlayHitReactMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

