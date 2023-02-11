// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterBase.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ACharacterBase::CanAttack()
{
	return false;
}

void ACharacterBase::Attack()
{
}

void ACharacterBase::AttackEnd()
{
}

void ACharacterBase::PlayAttackMontage()
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

