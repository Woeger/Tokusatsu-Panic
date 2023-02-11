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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Patrolling
	void PatrolTargetCheck();
	void CombatTargetCheck();

	//Damage
	virtual void GetHit_Implementation(const FVector& Impact) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	//Damage
	virtual void Death() override;

	//Patrolling
	bool InTargetRange(AActor* Target, double AcceptanceRadius);
	void MoveToTarget(AActor* Target);
	UFUNCTION()
	void OnSeen(APawn* Target);
	AActor* DecidePatrolTarget();

	//Montages
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

private:
	//Components
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarComponent;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	//Navigation
	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double ActiveCombatRange = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRange = 160.f;

	UPROPERTY(EditAnywhere)
	double ActivePatrolRange = 200.f;

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinish();

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
};
