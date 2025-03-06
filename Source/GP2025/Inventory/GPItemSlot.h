// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotStruct.h"
#include "Item/GPItemStruct.h"
#include "GPItemSlot.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPItemSlot : public UUserWidget
{
	GENERATED_BODY()


public:
    virtual void NativeConstruct() override;

    /** ���� ������ ��Ÿ���� ������ ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FSlotStruct SlotData;

    /** ���� ������ ������ �����͸� �������� �Լ� */
    UFUNCTION(BlueprintCallable, Category = "Item")
    FGPItemStruct& GetItemData();

    UFUNCTION(BlueprintCallable, Category = "Item")
    void EquipItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FGPItemStruct CurrentItem;


};
