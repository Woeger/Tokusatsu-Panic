// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TokusatsuPanic/DebugMacro.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"

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

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Enemy Attributes"));
	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Enemy Health"));
	HealthBarComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::GetHit(const FVector& Impact)
{
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(true);
	}

	if (Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact(Impact);
	}

	else
	{
		Death();
	}
}

void AEnemy::DirectionalHitReact(const FVector& Impact)
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

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarComponent)
	{
		Attributes->TakeDamage(DamageAmount);
		HealthBarComponent->SetHealthPercent(Attributes->GetHealthPercent());
	}

	CombatTarget = EventInstigator->GetPawn();
	return 0.0f;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(false);
	}

	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnSeen);
	}

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
}

void AEnemy::Death()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);

		const int32 RandomDeath = FMath::RandRange(1, 3);
		FName SectionName = FName();

		switch (RandomDeath)
		{
		case 1:
			SectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Dead1;
			break;
		case 2:
			SectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Dead2;
			break;
		case 3:
			SectionName = FName("Death3");
			DeathPose = EDeathPose::EDP_Dead3;
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}

	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(false);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(15.f);

}

void AEnemy::PlayHitReactMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
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
	if (Target->ActorHasTag(FName("PlayerCharacter")) && EnemyState == EEnemyState::EES_Patrolling)
	{
		EnemyState = EEnemyState::EES_Chasing;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = Target;
		MoveToTarget(CombatTarget);
	}
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

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState != EEnemyState::EES_Patrolling)
	{ 
		CombatTargetCheck();
	}

	else
	{
		PatrolTargetCheck();
	}

}

void AEnemy::PatrolTargetCheck()
{
	if (InTargetRange(PatrolTarget, ActivePatrolRange))
	{
		PatrolTarget = DecidePatrolTarget();

		const float TimerDelay = FMath::RandRange(5.f, 10.f);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinish, TimerDelay);
	}
}

void AEnemy::CombatTargetCheck()
{
	if (!InTargetRange(CombatTarget, ActiveCombatRange))
	{
		CombatTarget = nullptr;

		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 120.f;
		MoveToTarget(PatrolTarget);

		if (HealthBarComponent)
		{
			HealthBarComponent->SetVisibility(false);
		}
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

