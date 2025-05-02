// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/GPItemSlot.h"
#include "Item/GPItemStruct.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPInventory.generated.h"


/**
 * 
 */
UCLASS()
class GP2025_API UGPInventory : public UUserWidget
{
	GENERATED_BODY()
	
public :
	virtual void NativeConstruct() override;

	void AddItemToInventory(uint32 ItemID , uint8 ItemType , uint32 Quantity);
	void UseItemFromInventory(uint32 ItemID);

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UWidgetSwitcher* TabWidgetSwitcher;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UWrapBox* WeaponsWrapBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UWrapBox* ArmorsWrapBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UWrapBox* EatablesWrapBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UImage* PortraitImage;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* ItemDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UGPItemSlot> SlotClass;

	UPROPERTY()
	TArray<UGPItemSlot*> WeaponSlots;

	UPROPERTY()
	TArray<UGPItemSlot*> ArmorSlots;

	UPROPERTY()
	TArray<UGPItemSlot*> EatableSlots;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UTextBlock* MoneyText;

	void SetGold(int32 Amount);

	void SetPortraitRenderTarget(class UTextureRenderTarget2D* RenderTarget);
};
