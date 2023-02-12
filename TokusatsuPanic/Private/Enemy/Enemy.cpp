// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "TokusatsuPanic/DebugMacro.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensingComponent->SightRadius = 4000.f;
	PawnSensingComponent->SetPeripheralVisionAngle(45.f);

	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Enemy Health"));
	HealthBarComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState == EEnemyState::EES_Dead) return;

	if (EnemyState != EEnemyState::EES_Patrolling)
	{
		CombatTargetCheck();
	}

	else
	{
		PatrolTargetCheck();
	}

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	if (World && WeaponClass)
	{
		AWeapon* Weapon = World->SpawnActor<AWeapon>(WeaponClass);
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this);
		EquippedWeapon = Weapon;
	}

	ToggleHealthVisibility(false);

	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnSeen);
	}

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
}

//Components

void AEnemy::ToggleHealthVisibility(bool visibility)
{
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(visibility);
	}
}

//Attacking

void AEnemy::Attack()
{
	EnemyState = EEnemyState::EES_CombatEngaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack = InTargetRange(CombatTarget, AttackRange) &&
		EnemyState != EEnemyState::EES_Attacking &&
		EnemyState != EEnemyState::EES_CombatEngaged &&
		EnemyState != EEnemyState::EES_Dead;

	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_None;
	CombatTargetCheck();
}

void AEnemy::BeginAttack()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackDelay = FMath::RandRange(AttackDelayMin, AttackDelayMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackDelay);
}

//Damage

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (HealthBarComponent)
	{
		HealthBarComponent->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::GetHit_Implementation(const FVector& Impact)
{
	ToggleHealthVisibility(true);

	if (Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact(Impact);
	}

	else
	{
		Death();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	BeginChase();

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::Death()
{
	EnemyState = EEnemyState::EES_Dead;

	GetWorldTimerManager().ClearTimer(AttackTimer);

	int32 Selection = PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);
	
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	ToggleHealthVisibility(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetLifeSpan(LifeSpan);

}

//Patrolling

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	ToggleHealthVisibility(false);
}

bool AEnemy::InTargetRange(AActor* Target, double AcceptanceRadius)
{
	if (Target == nullptr) return false;
	const double Distance = (Target->GetActorLocation() - GetActorLocation()).Size();
	return Distance <= AcceptanceRadius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController && Target)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.f);
		EnemyController->MoveTo(MoveRequest);
	}
}

void AEnemy::OnSeen(APawn* Target)
{
	const bool bCanChase =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		Target->ActorHasTag(FName("PlayerCharacter"));

	if (bCanChase)
	{
		CombatTarget = Target;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		BeginChase();
	}
}

void AEnemy::BeginPatrol()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::BeginChase()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	MoveToTarget(CombatTarget);
}

AActor* AEnemy::DecidePatrolTarget()
{
	TArray<AActor*> ValidPatrolTargets;

	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidPatrolTargets.AddUnique(Target);
		}
	}

	const int32 PatrolTargetLength = ValidPatrolTargets.Num();
	if (PatrolTargetLength > 0)
	{
		const int32 RandomTarget = FMath::RandRange(0, PatrolTargetLength - 1);
		return ValidPatrolTargets[RandomTarget];
	}

	return nullptr;

}

void AEnemy::PatrolTimerFinish()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::PatrolTargetCheck()
{
	if (InTargetRange(PatrolTarget, ActivePatrolRange))
	{
		PatrolTarget = DecidePatrolTarget();

		const float TimerDelay = FMath::RandRange(PatrolDelayMin, PatrolDelayMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinish, TimerDelay);
	}
}

void AEnemy::CombatTargetCheck()
{
	//Outside of aggro range, lose interest in target
	if (!InTargetRange(CombatTarget, ActiveCombatRange))
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
		LoseInterest();

		if (EnemyState != EEnemyState::EES_CombatEngaged)
		{
			BeginPatrol();
		}
	}

	//Outside of attack range, continue to chase
	else if (!InTargetRange(CombatTarget, AttackRange) && EnemyState != EEnemyState::EES_Chasing)
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
		BeginChase();
	}

	//Inside of attack range, attack target
	else if (CanAttack())
	{
		BeginAttack();
	}
}

