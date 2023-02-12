// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "Characters/CharacterEnums.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;

UCLASS()
class TOKUSATSUPANIC_API AEnemy : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

	//Patrolling
	void PatrolTargetCheck();
	void CombatTargetCheck();

	//Damage
	virtual void GetHit_Implementation(const FVector& Impact) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	//Damage
	virtual void HandleDamage(float DamageAmount) override;
	virtual void Death() override;
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	//Attacking
	virtual void Attack() override;

	//Patrolling
	bool InTargetRange(AActor* Target, double AcceptanceRadius);
	void MoveToTarget(AActor* Target);
	UFUNCTION()
	void OnSeen(APawn* Target);
	AActor* DecidePatrolTarget();

	//Montages
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;

private:
	//Components
	void ToggleHealthVisibility(bool visibility);

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarComponent;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	//Weapons
	UPROPERTY(EditAnywhere, Category = "Weapons")
	TSubclassOf<class AWeapon> WeaponClass;

	//Combat
	void BeginAttack();

	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, Category = "Attacking")
	float AttackDelayMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Attacking")
	float AttackDelayMax = 1.f;

	//Navigation
	FTimerHandle PatrolTimer;
	void PatrolTimerFinish();
	void LoseInterest();
	void BeginPatrol();
	void BeginChase();

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double ActiveCombatRange = 500.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double AttackRange = 160.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double ActivePatrolRange = 200.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolSpeed = 120.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float ChaseSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolDelayMin = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolDelayMax = 10.f;

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;
};
