// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPInventory.h"
#include "Components/WrapBox.h"
#include "Components/WidgetSwitcher.h"
#include "Inventory/SlotStruct.h"
#include "Engine/DataTable.h"

void UGPInventory::AddItemToInventory(uint8 ItemType, uint32 Quantity)
{
    if (!ItemDataTable || !SlotClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemDataTable or SlotClass is NULL"));
        return;
    }

    // 디버깅: 전달받은 ItemType과 Quantity 확인
    UE_LOG(LogTemp, Warning, TEXT("AddItemToInventory - ItemType: %d | Quantity: %d"), ItemType, Quantity);

    FName RowName = FName(*FString::FromInt(static_cast<int32>(ItemType)));
    FGPItemStruct* ItemData = ItemDataTable->FindRow<FGPItemStruct>(RowName, TEXT(""));

    if (!ItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid ItemType or No Data Found for ItemType: %d"), static_cast<int32>(ItemType));
        return;
    }

    // 디버깅: 아이템 데이터 확인
    UE_LOG(LogTemp, Warning, TEXT("Item Found - ItemName: %s | Category: %d"),
        *ItemData->ItemName.ToString(), static_cast<int32>(ItemData->Category));

    UGPItemSlot* NewSlot = CreateWidget<UGPItemSlot>(GetWorld(), SlotClass);
    if (!NewSlot)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to Create WBP_Slot Widget"));
        return;
    }

    NewSlot->SlotData.ItemID.DataTable = ItemDataTable;
    NewSlot->SlotData.ItemID.RowName = RowName;
    NewSlot->SlotData.Quantity = Quantity;

    // 디버깅: 스위치문 실행 확인
    switch (ItemData->Category)
    {
    case ECategory::sword:
    case ECategory::bow:
        NewSlot->SlotData.ItemType = EItemTypes::Weapon;
        WeaponsWrapBox->AddChildToWrapBox(NewSlot);
        UE_LOG(LogTemp, Warning, TEXT("Added to WeaponsWrapBox"));
        break;

    case ECategory::helmet:
    case ECategory::chest:
        NewSlot->SlotData.ItemType = EItemTypes::Armor;
        ArmorsWrapBox->AddChildToWrapBox(NewSlot);
        UE_LOG(LogTemp, Warning, TEXT("Added to ArmorsWrapBox"));
        break;

    case ECategory::consumable:
    case ECategory::Gold:
    case ECategory::Quest:
        NewSlot->SlotData.ItemType = EItemTypes::Eatables;
        EatablesWrapBox->AddChildToWrapBox(NewSlot);
        UE_LOG(LogTemp, Warning, TEXT("Added to EatablesWrapBox"));
        break;

    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown ItemType - Item not added"));
        break;
    }
}
