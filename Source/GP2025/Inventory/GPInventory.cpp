// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPInventory.h"
#include "Components/WrapBox.h"
#include "Components/WidgetSwitcher.h"
#include "Inventory/SlotStruct.h"
#include "Engine/DataTable.h"


void UGPInventory::AddItemToInventory(uint8 ItemType, uint32 Quantity)
{
	if (!ItemDataTable || !SlotClass) return;

	FName RowName = FName(*FString::FromInt(static_cast<int32>(ItemType)));
	FGPItemStruct* ItemData = ItemDataTable->FindRow<FGPItemStruct>(RowName, TEXT(""));

	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ItemType or No Data Found for ItemType: %d"), static_cast<int32>(ItemType));
		return;
	}

	UGPItemSlot* NewSlot = CreateWidget<UGPItemSlot>(GetWorld(), SlotClass);
	if (!NewSlot)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Create WBP_Slot Widget"));
		return;
	}

	NewSlot->SlotData.ItemID.DataTable = ItemDataTable;
	NewSlot->SlotData.ItemID.RowName = RowName;
	NewSlot->SlotData.Quantity = Quantity;     

	switch (ItemData->Category)
	{
	case ECategory::sword :
	case ECategory::bow :
		NewSlot->SlotData.ItemType = EItemTypes::Weapon;
		WeaponsWrapBox->AddChildToWrapBox(NewSlot);
		break;
	case ECategory::helmet :
	case ECategory::chest :
		NewSlot->SlotData.ItemType = EItemTypes::Armor;
		ArmorsWrapBox->AddChildToWrapBox(NewSlot);
		break;
	case ECategory::consumable :
	case ECategory::Gold :
	case ECategory::Quest :
		NewSlot->SlotData.ItemType = EItemTypes::Eatables;
		EatablesWrapBox->AddChildToWrapBox(NewSlot);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown ItemType. Item will not be added."));
		break;
	}

}
