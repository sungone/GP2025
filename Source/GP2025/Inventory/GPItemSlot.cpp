// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPItemSlot.h"
#include "Character/GPCharacterPlayer.h"
#include "Inventory/GPInventory.h"
#include "Kismet/GameplayStatics.h"

void UGPItemSlot::NativeConstruct()
{
    Super::NativeConstruct();
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

void UGPItemSlot::EquipItem()
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
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("can't equip item"));
    }
}

