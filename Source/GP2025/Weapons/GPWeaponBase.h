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
	AGPWeaponBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

	void SetWeaponMesh(UStaticMesh* NewWeaponMesh);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UStaticMeshComponent* GetWeaponMesh() const { return Cast<UStaticMeshComponent>(WeaponMesh); }
};
