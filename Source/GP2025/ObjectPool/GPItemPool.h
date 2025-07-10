// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item/GPItem.h"
#include "GPItemPool.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPItemPool : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UWorld* InWorld, int32 InitialSize);
	AGPItem* Acquire();
	void Release(AGPItem* Item);

private:
	UPROPERTY()
	TArray<AGPItem*> Pool;

	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	TSubclassOf<AGPItem> ItemClass;
	
};
