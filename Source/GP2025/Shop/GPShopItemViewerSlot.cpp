// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop/GPShopItemViewerSlot.h"
#include "Components/Image.h"
#include "Shop/GPShop.h"
#include "Item/GPItemStruct.h"
#include "Inventory/GPInventory.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Kismet/GameplayStatics.h"

void UGPShopItemViewerSlot::NativeConstruct()
{

}

void UGPShopItemViewerSlot::SetItemData(const FGPItemStruct& InItem)
{
	CurrentItem = InItem;
	SetImage(); 
}

