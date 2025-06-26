// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/SlotStruct.h"
#include "Item/GPItemStruct.h"
#include "GPShopItemViewerSlot.generated.h"


class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class GP2025_API UGPShopItemViewerSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FGPItemStruct CurrentItem;

	void SetItemData(const FGPItemStruct& InItem);

	// UI ±¸¼º¿ë
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void SetImage();

};
