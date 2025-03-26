// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPWeaponBase.generated.h"

UCLASS()
class GP2025_API AGPWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

	// 충돌 감지를 위한 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UCapsuleComponent> CollisionComponent;

	void SetWeaponMesh(UStaticMesh* NewWeaponMesh);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UStaticMeshComponent* GetWeaponMesh() const { return Cast<UStaticMeshComponent>(WeaponMesh); }

public :
	virtual void AttackHitCheck() PURE_VIRTUAL(AGPWeaponBase::AttackHitCheck, );
};
