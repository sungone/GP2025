// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop/GPShop.h"
#include "Components/TextBlock.h"  
#include "Components/Button.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/GPCharacterNPC.h"
#include "Network/GPNetworkManager.h"
#include "Inventory/GPItemSlot.h"
#include "Engine/DataTable.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Inventory/GPInventory.h"
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

	// PopulateShopItems();
	if (UGPNetworkManager* Mgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		Mgr->OnBuyItemResult.AddDynamic(this, &UGPShop::HandleBuyItemResult);
	}
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

void UGPShop::HandleBuyItemResult(bool bSuccess, uint32 CurrentGold, const FString& Message)
{
	if (bSuccess)
	{
		UpdateMoneyText(CurrentGold);
		MyPlayer->CharacterInfo.Gold = CurrentGold;
	}

	if (ResultMessage)
	{
		ShowResultMessage(Message, 3.0f);
	}
}

void UGPShop::ShowResultMessage(const FString& Message, float Duration)
{
	if (!ResultMessage) return;

	ResultMessage->SetText(FText::FromString(Message));
	ResultMessage->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(
		HideResultMsgTimerHandle,
		this,
		&UGPShop::HideResultMessage,
		Duration,
		false
	);
}

void UGPShop::HideResultMessage()
{
	if (ResultMessage)
	{
		ResultMessage->SetVisibility(ESlateVisibility::Hidden);
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
	UE_LOG(LogTemp, Log, TEXT("PopulateShopItems Called - ShopType: %d"), static_cast<int32>(CurrentShopType));

	if (!ItemDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemDataTable is nullptr"));
		return;
	}

	if (!SlotClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SlotClass is nullptr"));
		return;
	}

	if (!BuyWrapBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemWrapBox is nullptr"));
		return;
	}

	BuyWrapBox->ClearChildren();

	TArray<FName> RowNames = ItemDataTable->GetRowNames();
	if (RowNames.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No items found in ItemDataTable"));
		return;
	}

	for (const FName& RowName : RowNames)
	{
		FGPItemStruct* ItemData = ItemDataTable->FindRow<FGPItemStruct>(RowName, TEXT("PopulateShopItems"));
		if (!ItemData) continue;

		UE_LOG(LogTemp, Log, TEXT("Item: %s | ShopType: %d"), *ItemData->ItemName.ToString(), static_cast<int32>(ItemData->ShopType));

		if (ItemData->ShopType == CurrentShopType)
		{
			UGPItemSlot* NewSlot = CreateWidget<UGPItemSlot>(this, SlotClass);
			if (!SlotClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("SlotClass is nullptr - Unable to create UGPItemSlot"));
				return;
			}

			if (!NewSlot) continue;

			NewSlot->SetOwningShop(this);
			NewSlot->SetOwningNPC(OwningNPC); 
			NewSlot->SlotOwnerType = ESlotOwnerType::Shop;
			NewSlot->SlotData.ItemID.DataTable = ItemDataTable;
			NewSlot->SlotData.ItemID.RowName = RowName;
			NewSlot->SlotData.Quantity = 1;
			NewSlot->CurrentItem = *ItemData;

			BuyWrapBox->AddChild(NewSlot);

			UE_LOG(LogTemp, Log, TEXT("Added Item: %s to Shop"), *ItemData->ItemName.ToString());
		}
	}

	BuyWrapBox->SetVisibility(ESlateVisibility::Visible);
}

void UGPShop::PopulateSellItems()
{
	if (!MyPlayer || !SellWrapBox || !SlotClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PopulateSellItems - Required components are null"));
		return;
	}

	SellWrapBox->ClearChildren();
	const TArray<UGPItemSlot*> InventorySlots = MyPlayer->UIManager->GetInventoryWidget()->GetAllInventoryItemData();

	for (UGPItemSlot* InventorySlot : InventorySlots)
	{
		if (!InventorySlot || InventorySlot->SlotData.Quantity <= 0) continue;

		UGPItemSlot* NewSlot = CreateWidget<UGPItemSlot>(this, SlotClass);
		if (!NewSlot) continue;

		NewSlot->SlotOwnerType = ESlotOwnerType::Sell;
		NewSlot->SlotData = InventorySlot->SlotData;
		NewSlot->CurrentItem = InventorySlot->CurrentItem;
		NewSlot->SetOwningShop(this);
		NewSlot->SetOwningNPC(OwningNPC);

		SellWrapBox->AddChild(NewSlot);

		UE_LOG(LogTemp, Log, TEXT("Added Sell Slot - %s | Quantity: %d"),
			*NewSlot->CurrentItem.ItemName.ToString(), NewSlot->SlotData.Quantity);
	}
}

void UGPShop::SetShopType(EShopType NewShopType)
{
	CurrentShopType = NewShopType;
	UE_LOG(LogTemp, Log, TEXT("SetShopType - CurrentShopType: %d"), static_cast<int32>(CurrentShopType));
}
