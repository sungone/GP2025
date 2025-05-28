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
    if (!SlotData.ItemID.DataTable) // ������ ���̺��� ������ ��ȯ X
    {
        UE_LOG(LogTemp, Warning, TEXT("Item DataTable is NULL"));
        CurrentItem = FGPItemStruct();
        return CurrentItem;
    }

    // ������ ���̺��� ������ ���� ��������
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

        // ���� �÷��̾ ã��
        AGPCharacterPlayer* Player = Cast<AGPCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (!Player)
        {
            UE_LOG(LogTemp, Warning, TEXT("No Player"));
            return;
        }

        // ���� ������ ���������� Ȯ��
        if (CurrentItem.Category == ECategory::helmet ||
            CurrentItem.Category == ECategory::chest ||
            CurrentItem.Category == ECategory::sword ||
            CurrentItem.Category == ECategory::bow)
        {
            UGPNetworkManager* NetworkManager = GetWorld()->GetGameInstance()->GetSubsystem<UGPNetworkManager>();
            if (NetworkManager)
            {
                if (!Player->AppearanceHandler) return;
                Player->AppearanceHandler->EquipItemOnCharacter(GetItemData());
                int32* FoundID = Player->EquippedItemIDs.Find(CurrentItem.Category);
                if (!FoundID)  return;

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
        UE_LOG(LogTemp, Warning, TEXT("Shop Slot Clicked"));

        if (ShopWidget)
        {
            ShopWidget->SetCurrentSlot(this);
            UE_LOG(LogTemp, Warning, TEXT("Shop's Current Slot set to: %s"), *GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("OwningShop is NULL"));
        }

        break;
    }

    case ESlotOwnerType::Sell:
    {
        UE_LOG(LogTemp, Warning, TEXT("Sell Slot Clicked"));

        if (ShopWidget)
        {
            ShopWidget->SetCurrentSlot(this);
            UE_LOG(LogTemp, Warning, TEXT("Sell Slot - CurrentSlot set to: %s"), *GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Sell Slot - ShopWidget is NULL"));
        }

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
                Inventory->HelmetViewerSlot->SlotData.ItemUniqueID = SlotData.ItemUniqueID;
            }
            break;

        case ECategory::chest:
            if (Inventory->ArmorViewerSlot)
            {
                Inventory->ArmorViewerSlot->SetSlotDataFromRowName(RowName);
                Inventory->ArmorViewerSlot->SlotData.ItemUniqueID = SlotData.ItemUniqueID;
            }
            break;

        case ECategory::sword:
        case ECategory::bow:
            if (Inventory->WeaponViewerSlot)
            {
                Inventory->WeaponViewerSlot->SetSlotDataFromRowName(RowName);
                Inventory->WeaponViewerSlot->SlotData.ItemUniqueID = SlotData.ItemUniqueID;
            }
            break;

        default:
            break;
        }
    }
}

void UGPItemSlot::SetOwningShop(UGPShop* InShop)
{
    if (InShop)
    {
        ShopWidget = InShop;
    }
}

void UGPItemSlot::SetOwningNPC(AGPCharacterNPC* InNPC)
{
    OwningNPC = InNPC;

    if (OwningNPC && OwningNPC->ShopWidget)
    {
        ShopWidget = Cast<UGPShop>(OwningNPC->ShopWidget);
        if (ShopWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("Successfully set ShopWidget in GPItemSlot from NPC: %s"), *OwningNPC->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OwningNPC or ShopWidget is null"));
    }
}

void UGPItemSlot::SetShopWidget(UGPShop* InShopWidget, AGPCharacterNPC* InNPC)
{
    if (InShopWidget)
    {
        ShopWidget = InShopWidget;
        OwningNPC = InNPC;

        UE_LOG(LogTemp, Warning, TEXT("Successfully set ShopWidget in GPItemSlot from NPC: %s"), *OwningNPC->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to set ShopWidget in GPItemSlot - InShopWidget is nullptr"));
    }
}
