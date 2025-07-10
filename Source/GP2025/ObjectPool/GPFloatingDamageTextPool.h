// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UI/GPFloatingDamageText.h"
#include "GPFloatingDamageTextPool.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPFloatingDamageTextPool : public UObject
{
	GENERATED_BODY()

public:
    void Initialize(UWorld* InWorld, int32 InitialSize);
    AGPFloatingDamageText* Acquire();
    void Release(AGPFloatingDamageText* DamageText);

private:
    UPROPERTY()
    TArray<AGPFloatingDamageText*> Pool;

    UPROPERTY()
    UWorld* World;

    TSubclassOf<AGPFloatingDamageText> DamageTextClass;
};
