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

	//Equipping
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* InOwner, APawn* InInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	//Hitting
	TArray<AActor*> IgnoreActors;

//Getters/Setters
public:
	FORCEINLINE UBoxComponent* GetWeaponHitbox() const { return WeaponHitbox; }

protected:

	virtual void BeginPlay();

	//Overlaps
	virtual void CollisionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void CollisionSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	void ExecuteGetHit(FHitResult& Hit);
	bool ActorIsSame(AActor* OtherActor);

	UFUNCTION()
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& Location);

private:

	//Box Tracing
	void BoxTrace(FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Weapon Parameters")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Parameters")
	bool bShowDebugBox = true;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* StartHitTrace;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* EndHitTrace;

	//Weapon properties
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* WeaponHitbox;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
};
