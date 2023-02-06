// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class TOKUSATSUPANIC_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, APawn* InInstigator);

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;

//Getters/Setters
public:
	FORCEINLINE UBoxComponent* GetWeaponHitbox() const { return WeaponHitbox; }

protected:

	virtual void BeginPlay();

	virtual void CollisionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void CollisionSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& Location);

private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* WeaponHitbox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* StartHitTrace;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* EndHitTrace;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
};
