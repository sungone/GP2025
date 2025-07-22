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
#include "Shop/GPShopItemViewerSlot.h"
#include "Inventory/GPInventory.h"
#include "Item/GPItemStruct.h"
#include "Kismet/GameplayStatics.h"
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

	if (SellButton)
	{
		SellButton->OnClicked.AddDynamic(this, &UGPShop::OnSellItemClicked);
	}

	if (!ItemDataTable)
	{
		ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Item/GPItemTable.GPItemTable"));
		if (!ItemDataTable)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load ItemDataTable!"));
		}
	}

	if (UGPNetworkManager* Mgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		Mgr->OnBuyItemResult.AddDynamic(this, &UGPShop::HandleBuyItemResult);
		Mgr->OnSellItemResult.AddDynamic(this, &UGPShop::HandleSellItemResult);
	}

	if (OwningNPC)
	{
		switch (OwningNPC->NPCType)
		{
		case ENPCType::SUITSHOP:
		case ENPCType::JUICESHOP:
			if (BuyWidgetSwitchButton)
			{
				BuyWidgetSwitchButton->SetVisibility(ESlateVisibility::Collapsed);
			}
			if (SellWidgetSwitchButton)
			{
				SellWidgetSwitchButton->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;

		default:
			break;
		}
	}
}

void UGPShop::NativeDestruct()
{
	Super::NativeDestruct();
	CurrentSlot = nullptr;
}

void UGPShop::OnShopExit()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}

	if (OwningNPC)
	{
		OwningNPC->ExitInteraction();
		// RemoveFromParent();
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
		const FString FailedMessage = TEXT("구매할 수 없습니다");

		if (ItemID == 0 && RowNameString != "0")
		{
			if (ClickSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
				UE_LOG(LogTemp, Log, TEXT("[Shop] Played ErrorSound (Invalid ItemID)"));
			}
			return;
		}

		if (MyPlayer->bIsGunnerCharacter() &&
			(ItemID == 4 || ItemID == 5 || ItemID == 6))
		{
			if (ResultMessage)
			{
				ShowResultMessage(FailedMessage, 3.0f);
			}

			if (ClickSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
				UE_LOG(LogTemp, Log, TEXT("[Shop] Played ErrorSound (Wrong class item for Gunner)"));
			}
			return;
		}

		if (!MyPlayer->bIsGunnerCharacter() &&
			(ItemID == 1 || ItemID == 2 || ItemID == 3))
		{
			if (ResultMessage)
			{
				ShowResultMessage(FailedMessage, 3.0f);
			}

			if (ClickSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
				UE_LOG(LogTemp, Log, TEXT("[Shop] Played ErrorSound (Wrong class item for Warrior)"));
			}
			return;
		}

		//if (ClickSound)
		//{
		//	UGameplayStatics::PlaySound2D(GetWorld(), SellAndBuySound);
		//	UE_LOG(LogTemp, Log, TEXT("[Shop] Played SellAndBuySound (Successful purchase attempt)"));
		//}

		const int32 Quantity = 1;
		NetworkMgr->SendMyShopBuyItem(ItemID, Quantity);

		UE_LOG(LogTemp, Log, TEXT("[Shop] BuyItem Sent to Server - ItemID: %d, Quantity: %d"), ItemID, Quantity);
	}
}
void UGPShop::HandleBuyItemResult(bool bSuccess, uint32 CurrentGold, const FString& Message)
{
	if (bSuccess)
	{
		UpdateMoneyText(CurrentGold);
		MyPlayer->CharacterInfo.Gold = CurrentGold;

		PopulateSellItems();
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

void UGPShop::OnSellItemClicked()
{
	if (!MyPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnSellItemClicked - MyPlayer is null"));
		return;
	}

	if (!CurrentSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnSellItemClicked - CurrentSlot is null, please select a slot again."));
		ShowResultMessage(TEXT("아이템을 선택해주세요."), 2.0f);
		return;
	}

	if (CurrentSlot->SlotOwnerType != ESlotOwnerType::Sell)
	{
		return;
	}

	if (CurrentSlot->SlotData.ItemUniqueIDs.Num() == 0)
	{
		return;
	}

	UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget();
	if (Inventory)
	{
		FName RowName = CurrentSlot->SlotData.ItemID.RowName;
		UGPItemSlot* InventorySlot = Inventory->FindSlotByRowName(RowName);
		if (InventorySlot)
		{
			if (Inventory->IsEquippedItem(InventorySlot, MyPlayer))
			{
				if (ResultMessage)
				{
					ShowResultMessage(TEXT("장착을 해제하고 판매해주세요"), 3.0f);
				}

				if (ClickSound)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
				}

				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OnSellItemClicked - No matching InventorySlot found."));
		}
	}

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SellAndBuySound);
	}

	// 첫 번째 고유 ID만 사용
	uint32 ItemUniqueIDToSell = CurrentSlot->SlotData.ItemUniqueIDs[0];

	if (UGPNetworkManager* NetworkMgr = MyPlayer->GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		NetworkMgr->SendMyShopSellItem(ItemUniqueIDToSell);
		UE_LOG(LogTemp, Log, TEXT("SellItem Sent to Server - ItemUniqueID: %d"), ItemUniqueIDToSell);
	}
}

void UGPShop::HandleSellItemResult(bool bSuccess, uint32 NewGold, const FString& Message)
{
	if (bSuccess)
	{
		UpdateMoneyText(NewGold);
		MyPlayer->CharacterInfo.Gold = NewGold;

		if (CurrentSlot)
		{
			if (UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget())
			{
				// 판매한 아이템 ID를 현재 슬롯에서 첫 번째 값으로 가져옴
				if (CurrentSlot->SlotData.ItemUniqueIDs.Num() > 0)
				{
					int32 SoldID = CurrentSlot->SlotData.ItemUniqueIDs[0];
					Inventory->RemoveItemByUniqueID(SoldID);  // 내부에서 UniqueIDs 배열과 Quantity를 업데이트하도록 구현됨
				}
			}

			CurrentSlot = nullptr;
		}

		// 판매 슬롯 재구성
		PopulateSellItems();
	}

	if (ResultMessage)
	{
		ShowResultMessage(Message, 3.0f);
	}
}

void UGPShop::SetCurrentSlot(UGPItemSlot* InSlot)
{
	if (InSlot)
	{
		CurrentSlot = InSlot;
		UE_LOG(LogTemp, Log, TEXT("CurrentSlot set to: %s"), *InSlot->GetName());

		if (ShopViewerSlot)
		{
			ShopViewerSlot->SetItemData(CurrentSlot->CurrentItem);
		}
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

void UGPShop::OpenShopAnimation()
{
	PlayAnimation(OpenAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void UGPShop::CloseShopAnimation()
{
	PlayAnimation(CloseAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}
