// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPItemSlot.h"
#include "Character/GPCharacterMyPlayer.h"
#include "Inventory/GPInventory.h"
#include "Shop/GPShop.h"
#include "Network/GPNetworkManager.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/GPEquippedItemSlot.h"
#include "Character/GPCharacterNPC.h"
#include "Components/TextBlock.h"

void UGPItemSlot::NativeConstruct()
{
    Super::NativeConstruct();
    InitializeInventoryWidget();
    InitializeShopWidget();
    CurrentItem = GetItemData();
}



FGPItemStruct& UGPItemSlot::GetItemData()
{
    if (!SlotData.ItemID.DataTable) // 데이터 테이블이 없으면 반환 X
    {
        UE_LOG(LogTemp, Warning, TEXT("Item DataTable is NULL"));
        CurrentItem = FGPItemStruct();
        return CurrentItem;
    }

    // 데이터 테이블에서 아이템 정보 가져오기
    FGPItemStruct* ItemData = SlotData.ItemID.DataTable->FindRow<FGPItemStruct>(SlotData.ItemID.RowName, TEXT(""));

    if (ItemData)
    {
        return *ItemData;
    }

    UE_LOG(LogTemp, Warning, TEXT("No Item Info : %s"), *SlotData.ItemID.RowName.ToString());
    CurrentItem = FGPItemStruct();
    return CurrentItem;
}

void UGPItemSlot::ClickItem()
{
    switch (SlotOwnerType)
    {

    case ESlotOwnerType::Inventory:

    {
        UE_LOG(LogTemp, Warning, TEXT("Success"));

        if (CurrentItem.Category == ECategory::None)
        {
            UE_LOG(LogTemp, Warning, TEXT("No Item"));
            return;
        }

        // 현재 플레이어를 찾음
        AGPCharacterPlayer* Player = Cast<AGPCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (!Player)
        {
            UE_LOG(LogTemp, Warning, TEXT("No Player"));
            return;
        }

        // 장착 가능한 아이템인지 확인
        if (CurrentItem.Category == ECategory::helmet ||
            CurrentItem.Category == ECategory::chest ||
            CurrentItem.Category == ECategory::sword ||
            CurrentItem.Category == ECategory::bow)
        {
            UGPNetworkManager* NetworkManager = GetWorld()->GetGameInstance()->GetSubsystem<UGPNetworkManager>();
            if (NetworkManager)
            {
                Player->AppearanceHandler->EquipItemOnCharacter(GetItemData());
                int32* FoundID = Player->EquippedItemIDs.Find(CurrentItem.Category);

                if (FoundID == nullptr)
                    return;

                if (*FoundID == -1)
                {
                    NetworkManager->SendMyEquipItem(SlotData.ItemUniqueID);
                }
                else
                {
                    NetworkManager->SendMyUnequipItem(*FoundID);
                    NetworkManager->SendMyEquipItem(SlotData.ItemUniqueID);
                }

                Player->EquippedItemIDs[CurrentItem.Category] = SlotData.ItemUniqueID;
                UpdatePlayerEquippedItemSlot(Player);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("EquipItem: NetworkManager is NULL"));
            }
        }
        else if (CurrentItem.Category == ECategory::consumable)
        {
            UGPNetworkManager* NetworkManager = GetWorld()->GetGameInstance()->GetSubsystem<UGPNetworkManager>();
            if (NetworkManager)
            {
                NetworkManager->SendMyUseItem(SlotData.ItemUniqueID);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("can't equip item"));
        }

        break;
    }

    case ESlotOwnerType::Shop:
    {
        break;
    }

    }
}

void UGPItemSlot::InitializeInventoryWidget()
{
    AGPCharacterMyplayer* Player = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (Player)
    {
        InventoryWidget = Player->UIManager->GetInventoryWidget();
        if (InventoryWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("Successfully Set InventoryWidget in GPItemSlot"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("InventoryWidget is NULL in GPCharacterMyPlayer"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to Get GPCharacterMyPlayer in GPItemSlot"));
    }
}

void UGPItemSlot::InitializeShopWidget()
{
    TArray<AActor*> FoundNPCs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGPCharacterNPC::StaticClass(), FoundNPCs);

    for (AActor* Actor : FoundNPCs)
    {
        AGPCharacterNPC* NPC = Cast<AGPCharacterNPC>(Actor);
        if (NPC && NPC->ShopWidget)
        {
            ShopWidget = Cast<UGPShop>(NPC->ShopWidget);
            if (ShopWidget)
            {
                UE_LOG(LogTemp, Warning, TEXT("Successfully set ShopWidget in GPItemSlot from NPC: %s"), *NPC->GetName());
                return;
            }
        }
    }
}

void UGPItemSlot::UpdateQuantityText()
{
    if (QuantityText)
    {
        QuantityText->SetText(FText::AsNumber(SlotData.Quantity));
    }
}
void UGPItemSlot::UpdatePlayerEquippedItemSlot(AGPCharacterPlayer* Player)
{
    if (Player)
    {
        AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(Player);
        if (!MyPlayer)
        {
            UE_LOG(LogTemp, Error, TEXT("[UpdatePlayerStatInfo] Failed to cast Player to MyPlayer"));
            return;
        }

        if (SlotData.ItemType == EItemTypes::Eatables)
        {
            UE_LOG(LogTemp, Warning, TEXT("[UpdatePlayerStatInfo] ItemType is 'Eatables' - skipping stat update"));
            return;
        }

        UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget();
        if (!Inventory)
        {
            UE_LOG(LogTemp, Error, TEXT("[UpdatePlayerStatInfo] InventoryWidget is NULL"));
            return;
        }

        FName RowName = SlotData.ItemID.RowName;
        FString ItemCategoryNameStr = UEnum::GetValueAsString(GetItemData().Category);

        switch (GetItemData().Category)
        {
        case ECategory::helmet:
            if (Inventory->HelmetViewerSlot)
            {
                Inventory->HelmetViewerSlot->SetSlotDataFromRowName(RowName);
            
            }
            break;

        case ECategory::chest:
            if (Inventory->ArmorViewerSlot)
            {
                Inventory->ArmorViewerSlot->SetSlotDataFromRowName(RowName);
              
            }
            break;

        case ECategory::sword:
        case ECategory::bow:
            if (Inventory->WeaponViewerSlot)
            {
                Inventory->WeaponViewerSlot->SetSlotDataFromRowName(RowName);
             
            }
            break;

        default:
            break;
        }
    }
}