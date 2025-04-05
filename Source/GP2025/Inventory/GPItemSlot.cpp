// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPItemSlot.h"
#include "Character/GPCharacterMyPlayer.h"
#include "Inventory/GPInventory.h"
#include "Network/GPNetworkManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

void UGPItemSlot::NativeConstruct()
{
    Super::NativeConstruct();
    InitializeInventoryWidget();
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
        Player->EquipItemOnCharacter(GetItemData());

        UGPNetworkManager* NetworkManager = GetWorld()->GetGameInstance()->GetSubsystem<UGPNetworkManager>();
        if (NetworkManager)
        {
            NetworkManager->SendPlayerEquipItem(SlotData.ItemUniqueID);
            UE_LOG(LogTemp, Warning, TEXT("EquipItem: Sent UniqueID [%d] to Server"), SlotData.ItemUniqueID);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("EquipItem: NetworkManager is NULL"));
        }
    }
    else if (CurrentItem.Category == ECategory::consumable || CurrentItem.Category == ECategory::Gold) 
    {
        UGPNetworkManager* NetworkManager = GetWorld()->GetGameInstance()->GetSubsystem<UGPNetworkManager>();
        if (NetworkManager)
        {
            NetworkManager->SendPlayerUseItem(SlotData.ItemUniqueID);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("can't equip item"));
    }
}

void UGPItemSlot::InitializeInventoryWidget()
{
    AGPCharacterMyplayer* Player = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (Player)
    {
        InventoryWidget = Player->GetInventoryWidget();
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

void UGPItemSlot::UpdateQuantityText()
{
    if (QuantityText)
    {
        QuantityText->SetText(FText::AsNumber(SlotData.Quantity));
    }
}
