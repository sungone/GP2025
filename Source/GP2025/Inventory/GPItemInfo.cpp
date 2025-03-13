// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPItemInfo.h"

void UGPItemInfo::InitializeItemInfo(const FSlotStruct& NewItem)
{
	Item = NewItem;

    if (Item.ItemID.RowName != NAME_None)
    {
        UE_LOG(LogTemp, Warning, TEXT("Item Info Initialized: %s"), *Item.ItemID.RowName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SlotData is Empty or Invalid"));
    }
}
