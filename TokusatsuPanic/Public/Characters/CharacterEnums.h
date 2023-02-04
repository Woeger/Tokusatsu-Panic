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
	EAS_Attack UMETA (DisplayName = "Attack")
};