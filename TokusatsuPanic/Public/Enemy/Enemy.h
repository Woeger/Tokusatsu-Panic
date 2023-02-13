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

	//Damage
	virtual void GetHit_Implementation(const FVector& Impact, AActor* HitTaker) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	//Damage
	virtual void HandleDamage(float DamageAmount) override;
	virtual void Death() override;
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	UPROPERTY(EditAnywhere)
	float LifeSpan = 15.f;

	//Attacking
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;

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

	//AI Navigation
	FTimerHandle PatrolTimer;
	void PatrolTimerFinish();
	void LoseInterest();
	void BeginPatrol();
	void BeginChase();
	void PatrolTargetCheck();
	void CombatTargetCheck();
	void MoveToTarget(AActor* Target);
	bool InTargetRange(AActor* Target, double AcceptanceRadius);
	AActor* DecidePatrolTarget();

	UFUNCTION()
	void OnSeen(APawn* Target);

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double ActiveCombatRange = 1150.f;
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

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;
};
