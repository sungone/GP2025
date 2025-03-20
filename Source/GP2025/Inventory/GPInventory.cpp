// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPInventory.h"
#include "Components/WrapBox.h"
#include "Components/WidgetSwitcher.h"
#include "Inventory/SlotStruct.h"
#include "Engine/DataTable.h"



void UGPInventory::NativeConstruct()
{
    Super::NativeConstruct();
}

void UGPInventory::AddItemToInventory(uint8 ItemType, uint32 Quantity)
{
    if (!ItemDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("AddItemToInventory - ItemDataTable is NULL!"));
        return;
    }

    if (!SlotClass)
    {
        UE_LOG(LogTemp, Error, TEXT("AddItemToInventory - SlotClass is NULL!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("AddItemToInventory - ItemType: %d | Quantity: %d"), ItemType, Quantity);

    FName RowName = FName(*FString::FromInt(static_cast<int32>(ItemType)));
    FGPItemStruct* ItemData = ItemDataTable->FindRow<FGPItemStruct>(RowName, TEXT(""));

    if (!ItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("AddItemToInventory - Invalid ItemType or No Data Found for ItemType: %d"), ItemType);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Item Found - ItemName: %s | Category: %d"),
        *ItemData->ItemName.ToString(), static_cast<int32>(ItemData->Category));

    
    TArray<UGPItemSlot*>* TargetArray = nullptr;

    switch (ItemData->Category)
    {
    case ECategory::sword:
    case ECategory::bow:
        TargetArray = &WeaponSlots;
        break;

    case ECategory::helmet:
    case ECategory::chest:
        TargetArray = &ArmorSlots;
        break;

    case ECategory::consumable:
    case ECategory::Gold:
    case ECategory::Quest:
        TargetArray = &EatableSlots;
        break;

    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown ItemType - Item not added"));
        return;
    }

    for (UGPItemSlot* ExistingSlot : *TargetArray)
    {
        if (ExistingSlot->SlotData.ItemID.RowName == RowName)
        {
            ExistingSlot->SlotData.Quantity += Quantity;
            UE_LOG(LogTemp, Warning, TEXT("Updated Existing Item - %s | Quantity: %d"),
                *ItemData->ItemName.ToString(), ExistingSlot->SlotData.Quantity);
            return;
        }
    }


    // 새로운 슬롯 추가
    UGPItemSlot* NewSlot = CreateWidget<UGPItemSlot>(GetWorld(), SlotClass);
    if (!NewSlot)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to Create WBP_Slot Widget"));
        return;
    }

    NewSlot->SlotData.ItemID.DataTable = ItemDataTable;
    NewSlot->SlotData.ItemID.RowName = RowName;
    NewSlot->SlotData.Quantity = Quantity;
    NewSlot->CurrentItem = *ItemData;

    TargetArray->Add(NewSlot);

    // UI 추가
    switch (ItemData->Category)
    {
    case ECategory::sword:
    case ECategory::bow:
        WeaponsWrapBox->AddChildToWrapBox(NewSlot);
        break;

    case ECategory::helmet:
    case ECategory::chest:
        ArmorsWrapBox->AddChildToWrapBox(NewSlot);
        break;

    case ECategory::consumable:
    case ECategory::Gold:
    case ECategory::Quest:
        EatablesWrapBox->AddChildToWrapBox(NewSlot);
        break;

    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown ItemType - Item not added"));
        break;
    }

    UE_LOG(LogTemp, Warning, TEXT("Item Successfully Added to Inventory - %s"), *ItemData->ItemName.ToString());
}

void UGPInventory::RemoveItemFromInventory(uint8 ItemType)
{
    TArray<UGPItemSlot*>* TargetArray = nullptr;

    switch (static_cast<ECategory>(ItemType))
    {
    case ECategory::sword:
    case ECategory::bow:
        TargetArray = &WeaponSlots;
        break;

    case ECategory::helmet:
    case ECategory::chest:
        TargetArray = &ArmorSlots;
        break;

    case ECategory::consumable:
    case ECategory::Gold:
    case ECategory::Quest:
        TargetArray = &EatableSlots;
        break;

    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown ItemType - Cannot Remove Item"));
        return;
    }

    for (int32 i = 0; i < TargetArray->Num(); ++i)
    {
        if ((*TargetArray)[i]->SlotData.ItemID.RowName == FName(*FString::FromInt(ItemType)))
        {
            (*TargetArray)[i]->RemoveFromParent();
            TargetArray->RemoveAt(i);

            UE_LOG(LogTemp, Warning, TEXT("Item Successfully Removed from Inventory"));
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Item Not Found in Inventory for ItemType: %d"), ItemType);
}
