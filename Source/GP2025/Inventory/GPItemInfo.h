// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotStruct.h" // SlotStruct 헤더 추가
#include "GPItemInfo.generated.h"


/**
 * 
 */
UCLASS()
class GP2025_API UGPItemInfo : public UUserWidget
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ExposeOnSpawn = "true"))
    FSlotStruct Item;

    UFUNCTION(BlueprintCallable, Category = "Item Info")
    void InitializeItemInfo(const FSlotStruct& NewSlotData);

};
