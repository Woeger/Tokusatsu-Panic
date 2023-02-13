#pragma once

UENUM(BlueprintType)
enum class EEquippedState : uint8
{
	EES_Unequipped UMETA(DisplayName = "Unequipped"),
	EES_Equipped1H UMETA(DisplayName = "Equipped One-Handed")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Idle UMETA(DisplayName = "Idle"),
	EAS_Hit UMETA(DisplayName = "Reacting to Hit"),
	EAS_Attack UMETA (DisplayName = "Attack"),
	EAS_Equipping UMETA (DisplayName = "Equipping"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Dead1 UMETA(DisplayName = "Dead 1"),
	EDP_Dead2 UMETA(DisplayName = "Dead 2"),
	EDP_Dead3 UMETA(DisplayName = "Dead 3"),

	EDP_MAX UMETA(DisplayName = "Max")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_None UMETA(DisplayName = "None"),
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_CombatEngaged UMETA(DisplayName = "CombatEngaged")
};