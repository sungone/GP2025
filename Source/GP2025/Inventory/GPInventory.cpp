// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPInventory.h"
#include "Components/WrapBox.h"
#include "Components/WidgetSwitcher.h"
#include "Inventory/SlotStruct.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterMyplayer.h"
#include "Inventory/GPStatInfo.h"
#include "Network/GPNetworkManager.h"
#include "Engine/DataTable.h"
#include "GPInventory.h"



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

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("GetWorld() returned nullptr in AddItemToInventory"));
        return;
    }

    UGPItemSlot* NewSlot = CreateWidget<UGPItemSlot>(World, SlotClass);
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
        NewSlot->SlotData.ItemType = EItemTypes::Weapon;
        TargetArray = &WeaponSlots;
        break;

    case ECategory::helmet:
    case ECategory::chest:
        NewSlot->SlotData.ItemType = EItemTypes::Armor;
        TargetArray = &ArmorSlots;
        break;

    case ECategory::consumable:
    case ECategory::Quest:
        NewSlot->SlotData.ItemType = EItemTypes::Eatables;
        TargetArray = &EatableSlots;
        break;

    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown ItemType - Item not added"));
        return;
    }

    if (!TargetArray) return;

    /// 이거 지운 이유: 아이템이 2개 이상이면 같은 아이템에서 개별적으로 UniqueID 에 접근할 수 없음. 대표되는 아이템에 모두 덮어쓰여짐 
    //for (UGPItemSlot* ExistingSlot : *TargetArray)
    //{
    //    if (ExistingSlot->SlotData.ItemID.RowName == RowName)
    //    {
    //        ExistingSlot->SlotData.Quantity += Quantity;
    //        UE_LOG(LogTemp, Warning, TEXT("Updated Existing Item - %s | Quantity: %d"),
    //            *ItemData->ItemName.ToString(), ExistingSlot->SlotData.Quantity);
    //        return;
    //    }
    //}

    if (!NewSlot) return;
    TargetArray->Add(NewSlot);

    if (!WeaponsWrapBox || !ArmorsWrapBox || !EatablesWrapBox)
    {
        UE_LOG(LogTemp, Error, TEXT("WrapBoxes are not properly initialized."));
        return;
    }

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

    UE_LOG(LogTemp, Warning, TEXT("Item Successfully Added to Inventory - %s [ItemUniqueID : %d]"), *ItemData->ItemName.ToString() , ItemID);
}

void UGPInventory::UseItemFromInventory(uint32 ItemID)
{
    auto HandleRemoveLogic = [ItemID](TArray<UGPItemSlot*>& SlotArray, UWrapBox* WrapBox) -> bool
        {
            for (int32 i = SlotArray.Num() - 1; i >= 0; --i)
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

void UGPInventory::RemoveItemByUniqueID(uint32 ItemUniqueID)
{
    auto RemoveFromInventory = [ItemUniqueID](TArray<UGPItemSlot*>& SlotArray, UWrapBox* WrapBox) -> bool
        {
            for (int32 i = SlotArray.Num() - 1; i >= 0; --i)
            {
                UGPItemSlot* Slot = SlotArray[i];

                if (Slot->SlotData.ItemUniqueID == ItemUniqueID)
                {
                    if (Slot->SlotData.Quantity > 1)
                    {
                        Slot->SlotData.Quantity--;
                        Slot->UpdateQuantityText();
                        UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByUniqueID] Item [%d] Quantity decreased to %d"), ItemUniqueID, Slot->SlotData.Quantity);
                    }
                    else
                    {
                        if (WrapBox)
                        {
                            WrapBox->RemoveChild(Slot);
                        }

                        Slot->RemoveFromParent();
                        SlotArray.RemoveAt(i);

                        UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByUniqueID] Item [%d] removed from inventory"), ItemUniqueID);
                    }
                    return true;
                }
            }
            return false;
        };

    if (RemoveFromInventory(WeaponSlots, WeaponsWrapBox)) return;
    if (RemoveFromInventory(ArmorSlots, ArmorsWrapBox)) return;
    if (RemoveFromInventory(EatableSlots, EatablesWrapBox)) return;

    UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByUniqueID] Item [%d] not found in inventory"), ItemUniqueID);
}

void UGPInventory::SetGold(int32 Amount)
{
    if (MoneyText)
    {
        MoneyText->SetText(FText::AsNumber(Amount));
    }
}

void UGPInventory::HandlePlayerStatUpdate()
{
    if (StatInfo && StatInfo->IsVisible())
    {
        StatInfo->UpdateStatInfo();
    }
}

TArray<UGPItemSlot*> UGPInventory::GetAllInventoryItemData() const
{
    TArray<UGPItemSlot*> AllSlots;
    AllSlots.Append(WeaponSlots);
    AllSlots.Append(ArmorSlots);
    AllSlots.Append(EatableSlots);
    return AllSlots;
}

