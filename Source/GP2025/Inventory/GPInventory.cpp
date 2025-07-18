// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPInventory.h"
#include "Components/WrapBox.h"
#include "Components/WidgetSwitcher.h"
#include "Inventory/SlotStruct.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Inventory/GPStatInfo.h"
#include "UI/GPChatBoxWidget.h"
#include "Network/GPNetworkManager.h"
#include "Engine/DataTable.h"
#include "GPInventory.h"



void UGPInventory::NativeConstruct()
{
    Super::NativeConstruct();
}

void UGPInventory::AddItemToInventory(uint32 ItemID , uint8 ItemType, uint32 Quantity)
{
    if (!ItemDataTable)
    {
        return;
    }

    if (!SlotClass)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("AddItemToInventory - ItemType: %d | Quantity: %d"), ItemType, Quantity);

    FName RowName = FName(*FString::FromInt(static_cast<int32>(ItemType)));
    FGPItemStruct* ItemData = ItemDataTable->FindRow<FGPItemStruct>(RowName, TEXT(""));

    if (!ItemData)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Item Found - ItemName: %s | Category: %d"),
        *ItemData->ItemName.ToString(), static_cast<int32>(ItemData->Category));

    // If Gold
    if (ItemData->Category == ECategory::Gold)
    {
        UGPNetworkManager* NetworkManager = GetWorld()->GetGameInstance()->GetSubsystem<UGPNetworkManager>();
        if (NetworkManager)
        {
            NetworkManager->SendMyUseItem(ItemID);
        }
        return;
    }

    TArray<UGPItemSlot*>* TargetArray = nullptr;
    EItemTypes ThisItemType = EItemTypes::Weapon;

    switch (ItemData->Category)
    {
    case ECategory::sword:
    case ECategory::bow:
        TargetArray = &WeaponSlots;
        break;

    case ECategory::helmet:
    case ECategory::chest:
        TargetArray = &ArmorSlots;
        ThisItemType = EItemTypes::Armor;
        break;

    case ECategory::consumable:
    case ECategory::Quest:
        TargetArray = &EatableSlots;
        ThisItemType = EItemTypes::Eatables;
        break;
    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown Item Category"));
        return;
    }

    if (!TargetArray) return;

        for (UGPItemSlot* ExistingSlot : *TargetArray)
    {
        if (ExistingSlot->SlotData.ItemID.RowName == RowName)
        {
            ExistingSlot->SlotData.Quantity += Quantity;
            ExistingSlot->SlotData.ItemUniqueIDs.Add(ItemID);
            ExistingSlot->UpdateQuantityText();
            UE_LOG(LogTemp, Warning, TEXT("Updated Existing Item - %s | Quantity: %d"),
                *ItemData->ItemName.ToString(), ExistingSlot->SlotData.Quantity);

            // 채팅창에 아이템 획득 내용 표시
            if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
            {
                UGPChatBoxWidget* ChatWidget = nullptr;

                if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(PC->GetPawn()))
                {
                    if (MyPlayer->UIManager)
                    {
                        ChatWidget = MyPlayer->UIManager->GetChatBoxWidget();
                    }
                }

                if (ChatWidget && ItemData)
                {
                    FString ItemName = ItemData->ItemName.ToString();
                    FString ChatMessage = FString::Printf(TEXT("를 획득하였습니다."), *ItemName);

                    UE_LOG(LogTemp, Log, TEXT("[ChatNotify] Sending message: %s"), *ChatMessage);

                    ChatWidget->AddChatMessage(EChatChannel::ItemSys, ItemName, ChatMessage);
                }
            }
            return;
        }
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    UGPItemSlot* NewSlot = CreateWidget<UGPItemSlot>(World, SlotClass);
    if (!NewSlot)
    {
        return;
    }

    NewSlot->SlotOwnerType = ESlotOwnerType::Inventory;
    NewSlot->CurrentItem = *ItemData;
    NewSlot->SlotData.ItemID.DataTable = ItemDataTable;
    NewSlot->SlotData.ItemID.RowName = RowName;
    NewSlot->SlotData.Quantity = Quantity;
    NewSlot->SlotData.ItemType = ThisItemType;
    NewSlot->SlotData.ItemUniqueIDs.Add(ItemID);
    
    TargetArray->Add(NewSlot);

    if (!WeaponsWrapBox || !ArmorsWrapBox || !EatablesWrapBox)
    {
        return;
    }

    // UI 추가
    switch (ItemData->Category)
    {
    case ECategory::sword:
    case ECategory::bow:
        WeaponsWrapBox->AddChildToWrapBox(NewSlot);
        break;

    case ECategory::helmet:
    case ECategory::chest:
        ArmorsWrapBox->AddChildToWrapBox(NewSlot);
        break;

    case ECategory::consumable:
    case ECategory::Quest:
        EatablesWrapBox->AddChildToWrapBox(NewSlot);
        break;

    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown ItemType - Item not added"));
        break;
    }

    UE_LOG(LogTemp, Warning, TEXT("Item Successfully Added to Inventory - %s [ItemUniqueID : %d]"), *ItemData->ItemName.ToString() , ItemID);

    // 채팅창에 아이템 획득 내용 표시
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        UGPChatBoxWidget* ChatWidget = nullptr;

        if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(PC->GetPawn()))
        {
            if (MyPlayer->UIManager)
            {
                ChatWidget = MyPlayer->UIManager->GetChatBoxWidget();
            }
        }

        if (ChatWidget && ItemData)
        {
            FString ItemName = ItemData->ItemName.ToString();
            FString ChatMessage = FString::Printf(TEXT("를 획득하였습니다."), *ItemName);

            UE_LOG(LogTemp, Log, TEXT("[ChatNotify] Sending message: %s"), *ChatMessage);

            ChatWidget->AddChatMessage(EChatChannel::ItemSys, ItemName, ChatMessage);
        }
    }
}

void UGPInventory::UseItemFromInventory(uint32 ItemID)
{
    auto HandleRemoveLogic = [ItemID](TArray<UGPItemSlot*>& SlotArray, UWrapBox* WrapBox) -> bool
        {
            for (int32 i = SlotArray.Num() - 1; i >= 0; --i)
            {
                UGPItemSlot* Slot = SlotArray[i];
                TArray<int32>& UniqueIDs = Slot->SlotData.ItemUniqueIDs;

                if (UniqueIDs.Contains(ItemID))
                {
                    UniqueIDs.RemoveSingle(ItemID);
                    Slot->SlotData.Quantity = UniqueIDs.Num();
                    Slot->UpdateQuantityText();

                    if (Slot->SlotData.Quantity == 0)
                    {
                        if (WrapBox)
                        {
                            WrapBox->RemoveChild(Slot);
                        }

                        Slot->RemoveFromParent();
                        SlotArray.RemoveAt(i);

                        UE_LOG(LogTemp, Warning, TEXT("Item [%d] completely removed from inventory"), ItemID);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Item [%d] quantity decreased. Remaining: %d"), ItemID, Slot->SlotData.Quantity);
                    }

                    return true;
                }
            }

            return false;
        };

    if (HandleRemoveLogic(WeaponSlots, WeaponsWrapBox)) return;
    if (HandleRemoveLogic(ArmorSlots, ArmorsWrapBox)) return;
    if (HandleRemoveLogic(EatableSlots, EatablesWrapBox)) return;

    UE_LOG(LogTemp, Warning, TEXT("UseItemFromInventory: Item [%d] not found in inventory"), ItemID);
}

void UGPInventory::RemoveItemByUniqueID(uint32 ItemUniqueID)
{
    auto RemoveFromInventory = [ItemUniqueID](TArray<UGPItemSlot*>& SlotArray, UWrapBox* WrapBox) -> bool
        {
            for (int32 i = SlotArray.Num() - 1; i >= 0; --i)
            {
                UGPItemSlot* Slot = SlotArray[i];
                TArray<int32>& UniqueIDs = Slot->SlotData.ItemUniqueIDs;

                if (UniqueIDs.Contains(ItemUniqueID))
                {
                    // 1. 서버로 보낸 ID 삭제
                    UniqueIDs.RemoveSingle(ItemUniqueID);

                    // 2. 수량 갱신
                    Slot->SlotData.Quantity = UniqueIDs.Num();
                    Slot->UpdateQuantityText();

                    // 3. 만약 UniqueIDs가 모두 없어졌다면 슬롯 제거
                    if (Slot->SlotData.Quantity == 0)
                    {
                        if (WrapBox)
                        {
                            WrapBox->RemoveChild(Slot);
                        }

                        Slot->RemoveFromParent();
                        SlotArray.RemoveAt(i);

                        UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByUniqueID] Item [%d] - Slot completely removed"), ItemUniqueID);
                    }
                    else
                    {
                        // 4. 남은 UniqueID들이 있다면 슬롯 유지
                        UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByUniqueID] Item [%d] removed. Remaining count: %d"),
                            ItemUniqueID, Slot->SlotData.Quantity);
                    }

                    return true;
                }
            }

            return false;
        };

    if (RemoveFromInventory(WeaponSlots, WeaponsWrapBox)) return;
    if (RemoveFromInventory(ArmorSlots, ArmorsWrapBox)) return;
    if (RemoveFromInventory(EatableSlots, EatablesWrapBox)) return;

    UE_LOG(LogTemp, Warning, TEXT("[RemoveItemByUniqueID] Item [%d] not found in inventory"), ItemUniqueID);
}


void UGPInventory::SetGold(int32 Amount)
{
    if (MoneyText)
    {
        MoneyText->SetText(FText::AsNumber(Amount));
    }
}

void UGPInventory::HandlePlayerStatUpdate()
{
    if (StatInfo && StatInfo->IsVisible())
    {
        StatInfo->UpdateStatInfo();
    }
}

TArray<UGPItemSlot*> UGPInventory::GetAllInventoryItemData() const
{
    TArray<UGPItemSlot*> AllSlots;
    AllSlots.Append(WeaponSlots);
    AllSlots.Append(ArmorSlots);
    AllSlots.Append(EatableSlots);
    return AllSlots;
}

bool UGPInventory::HasItemByType(uint8 ItemType) const
{
    FName RowName = FName(*FString::FromInt(static_cast<int32>(ItemType)));
    auto CheckItem = [RowName](const TArray<UGPItemSlot*>& Slots) -> bool
        {
            for (const auto& Slot : Slots)
            {
                if (Slot->SlotData.ItemID.RowName == RowName)
                {
                    return true; 
                }
            }
            return false;
        };

    if (CheckItem(WeaponSlots)) return true;
    if (CheckItem(ArmorSlots)) return true;
    if (CheckItem(EatableSlots)) return true;

    return false;
}

bool UGPInventory::IsEquippedItem(UGPItemSlot* SlotForSale, AGPCharacterPlayer* Player) const
{
    if (!SlotForSale || !Player)
    {
        return false;
    }

    ECategory ItemCategory = SlotForSale->CurrentItem.Category;

    if (ItemCategory == ECategory::consumable ||
        ItemCategory == ECategory::Gold ||
        ItemCategory == ECategory::Quest ||
        ItemCategory == ECategory::None)
    {
        return false;
    }

    int32* EquippedIDPtr = Player->EquippedItemIDs.Find(ItemCategory);
    if (!EquippedIDPtr || *EquippedIDPtr == -1)
    {
        return false;
    }

    int32 EquippedID = *EquippedIDPtr;

    if (SlotForSale->SlotData.ItemUniqueIDs.Contains(EquippedID))
    {
        UE_LOG(LogTemp, Warning, TEXT("[Inventory] Item %d (category %d) is currently equipped. Cannot sell."),
            EquippedID, static_cast<int32>(ItemCategory));
        return true;
    }

    return false;
}

UGPItemSlot* UGPInventory::FindSlotByRowName(FName RowName) const
{
    for (UGPItemSlot* SlotForFind : WeaponSlots)
    {
        if (!SlotForFind) continue;
        if (SlotForFind->SlotData.ItemID.RowName == RowName)
        {
            UE_LOG(LogTemp, Log, TEXT("[Inventory] Found slot in WeaponSlots for RowName: %s"), *RowName.ToString());
            return SlotForFind;
        }
    }

    for (UGPItemSlot* SlotForFind : ArmorSlots)
    {
        if (!SlotForFind) continue;
        if (SlotForFind->SlotData.ItemID.RowName == RowName)
        {
            UE_LOG(LogTemp, Log, TEXT("[Inventory] Found slot in ArmorSlots for RowName: %s"), *RowName.ToString());
            return SlotForFind;
        }
    }

    for (UGPItemSlot* SlotForFind : EatableSlots)
    {
        if (!SlotForFind) continue;
        if (SlotForFind->SlotData.ItemID.RowName == RowName)
        {
            UE_LOG(LogTemp, Log, TEXT("[Inventory] Found slot in EatableSlots for RowName: %s"), *RowName.ToString());
            return SlotForFind;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[Inventory] No slot found for RowName: %s"), *RowName.ToString());
    return nullptr;
}

void UGPInventory::OpenInventory()
{
    PlayAnimation(OpenAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void UGPInventory::CloseInventory()
{
    PlayAnimation(CloseAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}
