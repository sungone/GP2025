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

	if (BuyButton)
	{
		BuyButton->OnClicked.AddDynamic(this, &UGPShop::OnBuyItemClicked);
	}

	PopulateShopItems();
}

void UGPShop::NativeDestruct()
{
	Super::NativeDestruct();
	CurrentSlot = nullptr;
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
	if (!MyPlayer)
	{
		return;
	}

	if (!CurrentSlot)
	{
		return;
	}

	if (UGPNetworkManager* NetworkMgr = MyPlayer->GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		const FName RowName = CurrentSlot->SlotData.ItemID.RowName;
		const FString RowNameString = RowName.ToString();
		const int32 ItemID = FCString::Atoi(*RowNameString);

		if (ItemID == 0 && RowNameString != "0")
		{
			return;
		}

		const int32 Quantity = 1;
		NetworkMgr->SendMyShopBuyItem(ItemID, Quantity);

		UE_LOG(LogTemp, Log, TEXT("BuyItem Sent to Server - ItemID: %d, Quantity: %d"), ItemID, Quantity);
	}
}

void UGPShop::SetCurrentSlot(UGPItemSlot* InSlot)
{
	if (InSlot)
	{
		CurrentSlot = InSlot;
		UE_LOG(LogTemp, Log, TEXT("CurrentSlot set to: %s"), *InSlot->GetName());
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

		NewSlot->SetOwningShop(this);
		NewSlot->SlotOwnerType = ESlotOwnerType::Shop;
		NewSlot->SlotData.ItemID.DataTable = ItemDataTable;
		NewSlot->SlotData.ItemID.RowName = RowName;
		NewSlot->SlotData.Quantity = 1;
		NewSlot->CurrentItem = *ItemData;
		NewSlot->SlotData.ItemUniqueID = 1;

		// WrapBox에 추가
		ItemWrapBox->AddChild(NewSlot);
	}
}
