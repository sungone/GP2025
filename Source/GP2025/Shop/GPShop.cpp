// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop/GPShop.h"
#include "Components/TextBlock.h"  
#include "Components/Button.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/GPCharacterNPC.h"
#include "Network/GPNetworkManager.h"
#include "Inventory/GPItemSlot.h"
#include "Engine/DataTable.h"
#include "Item/GPItemStruct.h"
#include "Components/WrapBox.h"

void UGPShop::NativeConstruct()
{
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UGPShop::OnShopExit);
	}

	//static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Game/Item/GPItemTable.GPItemTable"));
	//if (DataTableRef.Succeeded())
	//{
	//	ItemDataTable = DataTableRef.Object;
	//}

	PopulateShopItems();
}

void UGPShop::OnShopExit()
{
	if (OwningNPC)
	{
		OwningNPC->ExitInteraction();
		RemoveFromParent();
	}
}

void UGPShop::UpdateMoneyText(int32 PlayerMoney)
{
	if (MoneyText)
	{
		MoneyText->SetText(FText::AsNumber(PlayerMoney));
	}
}

void UGPShop::SetMyPlayer(AGPCharacterMyplayer* Player)
{
	MyPlayer = Player;

	if (MyPlayer)
	{
		UpdateMoneyText(MyPlayer->CharacterInfo.Gold);
	}
}

void UGPShop::SetOwningNPC(AGPCharacterNPC* NPC)
{
	OwningNPC = NPC;
}

void UGPShop::OnBuyItemClicked()
{
	if (UGPNetworkManager* NetworkMgr = MyPlayer->GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		int32 ItemID = 1;
		int32 Quantity = 1;
		NetworkMgr->SendMyShopBuyItem(ItemID, Quantity);
	}
}

void UGPShop::PopulateShopItems()
{
	if (!ItemDataTable || !SlotClass || !ItemWrapBox) return;
	ItemWrapBox->ClearChildren();

	TArray<FName> RowNames = ItemDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		// 아이템 데이터 가져오기
		FGPItemStruct* ItemData = ItemDataTable->FindRow<FGPItemStruct>(RowName, TEXT("PopulateShopItems"));
		if (!ItemData) continue;

		// 슬롯 생성
		UGPItemSlot* NewSlot = CreateWidget<UGPItemSlot>(this, SlotClass);
		if (!NewSlot) continue;


		NewSlot->SlotOwnerType = ESlotOwnerType::Inventory;
		NewSlot->SlotData.ItemID.DataTable = ItemDataTable;
		NewSlot->SlotData.ItemID.RowName = RowName;
		NewSlot->SlotData.Quantity = 1;
		NewSlot->CurrentItem = *ItemData;
		NewSlot->SlotData.ItemUniqueID = 1;

		// WrapBox에 추가
		ItemWrapBox->AddChild(NewSlot);
	}
}
