// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPItemSlot.h"
#include "Character/GPCharacterMyPlayer.h"
#include "Inventory/GPInventory.h"
#include "Kismet/GameplayStatics.h"

void UGPItemSlot::NativeConstruct()
{
    Super::NativeConstruct();
    InitializeInventoryWidget();
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

void UGPItemSlot::EquipItem()
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
        Player->EquipItemOnCharacter(GetItemData());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("can't equip item"));
    }
}

void UGPItemSlot::InitializeInventoryWidget()
{
    // �÷��̾� ã��
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController) return;

    AGPCharacterMyplayer* Player = Cast<AGPCharacterMyplayer>(PlayerController->GetPawn());
    if (!Player)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to Cast to GPCharacterMyPlayer in GPItemSlot"));
        return;
    }

    // InventoryWidget ����
    InventoryWidget = Player->GetInventoryWidget();
    if (InventoryWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("Successfully Referenced InventoryWidget in GPItemSlot"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to Reference InventoryWidget in GPItemSlot"));
    }
}

