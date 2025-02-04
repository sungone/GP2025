// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GPItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Helmet = 0,
	Chest ,
	Potion,
	Exp
};

/**
 * 
 */
UCLASS()
class GP2025_API UGPItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public :
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Type")
	EItemType Type;
};
