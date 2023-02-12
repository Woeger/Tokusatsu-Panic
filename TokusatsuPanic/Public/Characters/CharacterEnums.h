#pragma once

UENUM(BlueprintType)
enum class EEquippedState : uint8
{
	EES_Unequipped UMETA(DisplayName = "Unequipped"),
	EES_Equipped1H UMETA(DisplayName = "Equipped One-Handed"),
	EES_Equipped2H UMETA(DisplayName = "Equipped Two-Handed")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Idle UMETA(DisplayName = "Idle"),
	EAS_Attack UMETA (DisplayName = "Attack"),
	EAS_Equipping UMETA (DisplayName = "Equipping")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Dead1 UMETA(DisplayName = "Dead 1"),
	EDP_Dead2 UMETA(DisplayName = "Dead 2"),
	EDP_Dead3 UMETA(DisplayName = "Dead 3")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_CombatEngaged UMETA(DisplayName = "CombatEngaged")
};