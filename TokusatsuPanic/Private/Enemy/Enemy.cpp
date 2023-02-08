// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TokusatsuPanic/DebugMacro.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

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

	EnemyController = Cast<AAIController>(GetController());

	if (EnemyController && PatrolTarget)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(PatrolTarget);
		MoveRequest.SetAcceptanceRadius(10.f);

		FNavPathSharedPtr NavPath;

		EnemyController->MoveTo(MoveRequest, &NavPath);

		TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
	}
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
	const double Distance = (Target->GetActorLocation() - GetActorLocation()).Size();
	return Distance <= AcceptanceRadius;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget)
	{
		if (!InTargetRange(CombatTarget, ActiveCombatRange))
		{
			CombatTarget = nullptr;

			if (HealthBarComponent)
			{
				HealthBarComponent->SetVisibility(false);
			}
		}

	}

	if (PatrolTarget && EnemyController)
	{
		if (InTargetRange(PatrolTarget, ActivePatrolRange))
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
				AActor* Target = ValidPatrolTargets[RandomTarget];
				PatrolTarget = Target;

				FAIMoveRequest MoveRequest;
				MoveRequest.SetGoalActor(PatrolTarget);
				MoveRequest.SetAcceptanceRadius(10.f);

				FNavPathSharedPtr NavPath;

				EnemyController->MoveTo(MoveRequest, &NavPath);

				TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
			}
		}
	}

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

