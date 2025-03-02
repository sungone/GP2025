// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GPMoneyStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGPMoneyStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()


public :

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Money")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Money")
	int32 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Money")
	UStaticMesh* Mesh;
};
