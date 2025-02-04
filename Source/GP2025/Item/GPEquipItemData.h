// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/GPItemData.h"
#include "GPEquipItemData.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPEquipItemData : public UGPItemData
{
	GENERATED_BODY()
	
public :
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<USkeletalMesh> EquipMesh;
};
