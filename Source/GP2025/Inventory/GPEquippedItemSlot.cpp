// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPEquippedItemSlot.h"
#include "Character/GPCharacterMyPlayer.h"
#include "Inventory/GPInventory.h"
#include "Shop/GPShop.h"
#include "Network/GPNetworkManager.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterNPC.h"
#include "Components/TextBlock.h"

void UGPEquippedItemSlot::NativeConstruct()
{
    Super::NativeConstruct();
    InitializeInventoryWidget();
    CurrentItem = GetItemData();
}

FGPItemStruct& UGPEquippedItemSlot::GetItemData()
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

void UGPEquippedItemSlot::ClickEquippedItemSlot()
{
    AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!MyPlayer)
        return;

    UGPNetworkManager* NetworkManager = GetWorld()->GetGameInstance()->GetSubsystem<UGPNetworkManager>();
    if (!NetworkManager)
        return;

    // 1. UniqueID 중 하나 선택 (첫 번째로)
    if (SlotData.ItemUniqueIDs.Num() > 0)
    {
        int32 UnequipID = SlotData.ItemUniqueIDs[0];
        NetworkManager->SendMyUnequipItem(UnequipID);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UGPEquippedItemSlot: No UniqueID to unequip."));
        return;
    }

    // 2. 외형 해제
    if (MyPlayer->AppearanceHandler)
    {
        MyPlayer->AppearanceHandler->UnequipItemFromCharacter(CurrentItem.Category);
    }

    // 3. 데이터 및 UI 초기화
    SlotData = FSlotStruct();
    CurrentItem = FGPItemStruct();
    SetImage();
}

void UGPEquippedItemSlot::InitializeInventoryWidget()
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

void UGPEquippedItemSlot::SetSlotDataFromRowName(FName InRowName)
{
    if (!InventoryWidget || !InventoryWidget->ItemDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("SetSlotDataFromRowName: InventoryWidget or ItemDataTable is NULL"));
        return;
    }

    SlotData.ItemID.DataTable = InventoryWidget->ItemDataTable;

    FGPItemStruct* FoundItem = SlotData.ItemID.DataTable->FindRow<FGPItemStruct>(InRowName, TEXT("SetSlotDataFromRowName"));

    if (!FoundItem)
    {
        UE_LOG(LogTemp, Error, TEXT("SetSlotDataFromRowName: No item found for RowName [%s]"), *InRowName.ToString());
        CurrentItem = FGPItemStruct();
        return;
    }

    SlotData.ItemID.RowName = InRowName;
    CurrentItem = *FoundItem;

    SetImage();

    UE_LOG(LogTemp, Log, TEXT("SetSlotDataFromRowName: SlotData set with RowName [%s], Name: %s"),
        *InRowName.ToString(), *CurrentItem.ItemName.ToString());
}