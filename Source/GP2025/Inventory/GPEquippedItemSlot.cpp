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

void UGPEquippedItemSlot::ClickItem()
{

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

void UGPEquippedItemSlot::SetSlotDataFromItemType(uint8 InItemType)
{
    if (!SlotData.ItemID.DataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("SetSlotDataFromItemType: DataTable is NULL"));
        return;
    }

    // ItemType을 RowName으로 사용 (예: "1", "2", "3", ...)
    FName RowName = FName(*FString::FromInt(InItemType));
    FGPItemStruct* FoundItem = SlotData.ItemID.DataTable->FindRow<FGPItemStruct>(RowName, TEXT("SetSlotDataFromItemType"));

    if (!FoundItem)
    {
        UE_LOG(LogTemp, Error, TEXT("SetSlotDataFromItemType: No item found for ItemType [%d]"), InItemType);
        CurrentItem = FGPItemStruct();
        return;
    }

    // SlotData 세팅
    SlotData.ItemID = FDataTableRowHandle();
    SlotData.ItemID.DataTable = InventoryWidget->ItemDataTable;
    SlotData.ItemID.RowName = FName(*FString::FromInt(InItemType));

    // 아이템 정보 캐싱
    CurrentItem = *FoundItem;

    SetImage();

    UE_LOG(LogTemp, Log, TEXT("SetSlotDataFromItemType: SlotData set with ItemType [%d], Name: %s"), InItemType, *CurrentItem.ItemName.ToString());
}
