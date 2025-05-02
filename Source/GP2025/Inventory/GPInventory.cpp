// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPInventory.h"
#include "Components/WrapBox.h"
#include "Components/WidgetSwitcher.h"
#include "Inventory/SlotStruct.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "Engine/DataTable.h"



void UGPInventory::NativeConstruct()
{
    Super::NativeConstruct();
}

void UGPInventory::AddItemToInventory(uint32 ItemID , uint8 ItemType, uint32 Quantity)
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

    UGPItemSlot* NewSlot = CreateWidget<UGPItemSlot>(GetWorld(), SlotClass);
    if (!NewSlot)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to Create WBP_Slot Widget"));
        return;
    }

    NewSlot->SlotOwnerType = ESlotOwnerType::Inventory;
    NewSlot->SlotData.ItemID.DataTable = ItemDataTable;
    NewSlot->SlotData.ItemID.RowName = RowName;
    NewSlot->SlotData.Quantity = Quantity;
    NewSlot->CurrentItem = *ItemData;
    NewSlot->SlotData.ItemUniqueID = ItemID;

    // 만약 Gold 라면 바로 Use 해서 Money 업데이트
    if (ItemData->Category == ECategory::Gold)
    {
        UGPNetworkManager* NetworkManager = GetWorld()->GetGameInstance()->GetSubsystem<UGPNetworkManager>();
        if (NetworkManager)
        {
            NetworkManager->SendMyUseItem(NewSlot->SlotData.ItemUniqueID);
        }
        return;
    }
    
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
    case ECategory::Quest:
        EatablesWrapBox->AddChildToWrapBox(NewSlot);
        break;

    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown ItemType - Item not added"));
        break;
    }

    UE_LOG(LogTemp, Warning, TEXT("Item Successfully Added to Inventory - %s"), *ItemData->ItemName.ToString());
}

void UGPInventory::UseItemFromInventory(uint32 ItemID)
{
    auto HandleRemoveLogic = [ItemID](TArray<UGPItemSlot*>& SlotArray, UWrapBox* WrapBox) -> bool
        {
            for (int32 i = 0; i < SlotArray.Num(); ++i)
            {
                UGPItemSlot* Slot = SlotArray[i];

                if (Slot->SlotData.ItemUniqueID == ItemID)
                {
                    if (Slot->SlotData.Quantity > 1)
                    {
                        Slot->SlotData.Quantity--;
                        Slot->UpdateQuantityText();
                        UE_LOG(LogTemp, Warning, TEXT("Item [%d] Quantity decreased to %d"), ItemID, Slot->SlotData.Quantity);
                    }
                    else
                    {
                        if (WrapBox)
                        {
                            WrapBox->RemoveChild(Slot);
                        }

                        Slot->RemoveFromParent();
                        SlotArray.RemoveAt(i);

                        UE_LOG(LogTemp, Warning, TEXT("Item [%d] completely removed from inventory"), ItemID);
                    }
                    return true;
                }
            }

            return false;
        };

    // 카테고리별로 탐색
    if (HandleRemoveLogic(WeaponSlots, WeaponsWrapBox)) return;
    if (HandleRemoveLogic(ArmorSlots, ArmorsWrapBox)) return;
    if (HandleRemoveLogic(EatableSlots, EatablesWrapBox)) return;

    UE_LOG(LogTemp, Warning, TEXT("RemoveItemFromInventory: Item [%d] not found in inventory"), ItemID);
}

void UGPInventory::SetGold(int32 Amount)
{
    if (MoneyText)
    {
        MoneyText->SetText(FText::AsNumber(Amount));
    }
}

