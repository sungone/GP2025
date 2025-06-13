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
	void RemoveItemByUniqueID(uint32 ItemUniqueID);

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UWidgetSwitcher* TabWidgetSwitcher;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UWrapBox* WeaponsWrapBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UWrapBox* ArmorsWrapBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
	class UWrapBox* EatablesWrapBox;

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

	// Stat Info 처리
	UPROPERTY(meta = (BindWidget))
	class UGPStatInfo* StatInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGPEquippedItemSlot> WeaponViewerSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGPEquippedItemSlot> HelmetViewerSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGPEquippedItemSlot> ArmorViewerSlot;

	UFUNCTION()
	void HandlePlayerStatUpdate();


	// 플레이어가 가지고 있는 모든 아이템 가져오기 //
	UFUNCTION(BlueprintCallable)
	TArray<UGPItemSlot*> GetAllInventoryItemData() const;

	// 아이템이 있는 지 확인
	UFUNCTION(BlueprintCallable)
	bool HasItemByType(uint8 ItemType) const;
};
