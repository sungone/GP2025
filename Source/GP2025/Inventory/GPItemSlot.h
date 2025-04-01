// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotStruct.h"
#include "Item/GPItemStruct.h"
#include "GPItemSlot.generated.h"


class UGPInventory;
class AGPCharacterMyplayer;

/**
 * 
 */
UCLASS()
class GP2025_API UGPItemSlot : public UUserWidget
{
	GENERATED_BODY()


public:
    virtual void NativeConstruct() override;

    // Expose on Spawn �Ű����� �߰�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ExposeOnSpawn = "true"))
    FSlotStruct SlotData;

    /** ���� ������ ������ �����͸� �������� �Լ� */
    UFUNCTION(BlueprintCallable, Category = "Item")
    FGPItemStruct& GetItemData();



    UFUNCTION(BlueprintCallable, Category = "Item")
    void ClickItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FGPItemStruct CurrentItem;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    UGPInventory* InventoryWidget;

    void InitializeInventoryWidget();
};
