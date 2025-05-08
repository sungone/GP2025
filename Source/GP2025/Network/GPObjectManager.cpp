// Fill out your copyright notice in the Description page of Project Settings.﻿

#include "Network/GPObjectManager.h"
#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterMonster.h"
#include "Item/GPItemStruct.h"
#include "Item/GPItem.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Character/Modules/GPCharacterCombatHandler.h"
#include "Character/Modules/GPPlayerEffectHandler.h"
#include "Skill/GPSkillCoolDownHandler.h"
#include "UI/GPFloatingDamageText.h"
#include "Network/GPGameInstance.h"
#include "Inventory/GPInventory.h"
#include "UI/GPInGameWidget.h"
#include "UI/GPQuestListEntryWidget.h"
#include "UI/GPQuestListWidget.h"
#include "Network/GPNetworkManager.h"
#include "Inventory/GPEquippedItemSlot.h"
#include "Kismet/GameplayStatics.h"
#include "GPObjectManager.h"

void UGPObjectManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	World = GetWorld();
	OtherPlayerClass = AGPCharacterPlayer::StaticClass();
	MonsterClass = AGPCharacterMonster::StaticClass();
}

void UGPObjectManager::Deinitialize()
{
	Super::Deinitialize();
}

void UGPObjectManager::SetMyPlayer(AGPCharacterPlayer* InMyPlayer)
{
	MyPlayer = Cast<AGPCharacterMyplayer>(InMyPlayer);
}

void UGPObjectManager::AddMyPlayer(const FInfoData& PlayerInfo)
{
	if (World == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Invaild World"));
		FGenericPlatformMisc::RequestExit(false);
	}

	FVector SpawnLocation(PlayerInfo.Pos);
	FRotator SpawnRotation(0, PlayerInfo.Yaw, 0);

	if (MyPlayer == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Invaild MyPlayer"));
		FGenericPlatformMisc::RequestExit(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("Login Success [%d] (%f,%f,%f)(%f)"),
		PlayerInfo.ID, PlayerInfo.Pos.X, PlayerInfo.Pos.Y, PlayerInfo.Pos.Z, PlayerInfo.Yaw);

	MyPlayer->SetCharacterInfo(PlayerInfo);
	MyPlayer->SetCharacterType(PlayerInfo.CharacterType);
	MyPlayer->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	if (MyPlayer->UIHandler)
	{
		MyPlayer->SetNameByCharacterInfo();
	}
	auto Player = Cast<AGPCharacterPlayer>(MyPlayer);
	Players.Add(PlayerInfo.ID, Player);
}


void UGPObjectManager::AddPlayer(const FInfoData& PlayerInfo)
{
	if (World == nullptr)
		return;

	FVector SpawnLocation(PlayerInfo.Pos);
	FRotator SpawnRotation(0, PlayerInfo.Yaw, 0);

	AGPCharacterPlayer* Player = World->SpawnActor<AGPCharacterPlayer>(OtherPlayerClass, SpawnLocation, SpawnRotation);

	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed Add other player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.Pos.X, PlayerInfo.Pos.Y, PlayerInfo.Pos.Z, PlayerInfo.Yaw);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Add other player [%d] (%f,%f,%f)(%f)"),
		PlayerInfo.ID, PlayerInfo.Pos.X, PlayerInfo.Pos.Y, PlayerInfo.Pos.Z, PlayerInfo.Yaw);

	Player->SetCharacterInfo(PlayerInfo);
	Player->SetCharacterType(PlayerInfo.CharacterType);
	if (Player->UIHandler)
	{
		Player->SetNameByCharacterInfo();
	}
	Player->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	Players.Add(PlayerInfo.ID, Player);
}

void UGPObjectManager::RemovePlayer(int32 PlayerID)
{
	if (Players.Contains(PlayerID))
	{
		Players[PlayerID]->Destroy();
		Players.Remove(PlayerID);
	}
}

void UGPObjectManager::UpdatePlayer(const FInfoData& PlayerInfo)
{
	auto PlayerID = PlayerInfo.ID;
	if (Players.Contains(PlayerID))
	{
		Players[PlayerID]->SetCharacterInfo(PlayerInfo);
	}
}

void UGPObjectManager::PlayerUseSkill(int32 PlayerID, ESkillGroup SkillGID)
{
	if (Players.Contains(PlayerID))
	{
		if (SkillGID == ESkillGroup::HitHard || SkillGID == ESkillGroup::Throwing)
		{
			Players[PlayerID]->CharacterInfo.AddState(STATE_SKILL_Q);
			Players[PlayerID]->CombatHandler->PlayQSkillMontage();
		}
		else if (SkillGID == ESkillGroup::Clash || SkillGID == ESkillGroup::FThrowing)
		{
			Players[PlayerID]->CharacterInfo.AddState(STATE_SKILL_E);
			Players[PlayerID]->CombatHandler->PlayESkillMontage();
		}
		else if (SkillGID == ESkillGroup::Whirlwind || SkillGID == ESkillGroup::Anger)
		{
			Players[PlayerID]->CharacterInfo.AddState(STATE_SKILL_R);
			Players[PlayerID]->CombatHandler->PlayRSkillMontage();
		}
	}
}

void UGPObjectManager::DamagedPlayer(const FInfoData& PlayerInfo)
{
	auto PlayerID = PlayerInfo.ID;
	if (Players.Contains(PlayerID))
	{
		Players[PlayerID]->SetCharacterInfo(PlayerInfo);
	}
}

void UGPObjectManager::HandlePlayerDeath(int32 playerId)
{
	//AGPCharacterPlayer* TargetPlayer = Players.FindRef(playerId);
	//if (!TargetPlayer) return;

	//TargetPlayer->CombatHandler->PlayDeadAnimation();
	//FTimerHandle HideTimerHandle;
	//TargetPlayer->GetWorldTimerManager().SetTimer(HideTimerHandle, [TargetPlayer]()
	//	{
	//		TargetPlayer->SetActorHiddenInGame(true);
	//		TargetPlayer->SetActorEnableCollision(false);
	//	}, 1.f, false);

	//if (TargetPlayer == MyPlayer)
	//{
	//	if (MyPlayer->UIManager)
	//	{
	//		MyPlayer->UIManager->ShowDeadScreen();
	//	}
	//}
}

void UGPObjectManager::SkillUnlock(ESkillGroup SkillGID)
{
	if (MyPlayer->EffectHandler)
		MyPlayer->EffectHandler->PlaySkillUnlockEffect();
}

void UGPObjectManager::SkillUpgrade(ESkillGroup SkillGID)
{
	// Todo : UI 처리
}

void UGPObjectManager::LevelUp(const FInfoData& PlayerInfo)
{
	if (MyPlayer->EffectHandler)
		MyPlayer->EffectHandler->PlayLevelUpEffect();
}

void UGPObjectManager::AddMonster(const FInfoData& MonsterInfo)
{
	if (World == nullptr)
		return;

	FVector SpawnLocation(MonsterInfo.Pos);
	FRotator SpawnRotation(0, MonsterInfo.Yaw, 0);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AGPCharacterMonster* Monster = World->SpawnActor<AGPCharacterMonster>(MonsterClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Monster == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn monster [%d] at location (%f, %f, %f)."),
			MonsterInfo.ID, SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawned Monster [%d] at (%f, %f, %f) with rotation (%f)."),
		MonsterInfo.ID, SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z, SpawnRotation.Yaw);

	Monster->SetCharacterInfo(MonsterInfo);
	Monster->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	Monster->SetCharacterType(MonsterInfo.CharacterType);

	if (Monster->UIHandler)
	{
		Monster->SetNameByCharacterInfo();
	}
	Monsters.Add(MonsterInfo.ID, Monster);
}

void UGPObjectManager::RemoveMonster(int32 MonsterID)
{
	UE_LOG(LogTemp, Warning, TEXT("Remove monster [%d]"), MonsterID);
	if (Monsters.Contains(MonsterID))
	{
		Monsters[MonsterID]->CombatHandler->HandleDeath();
		Monsters.Remove(MonsterID);
	}
}

void UGPObjectManager::UpdateMonster(const FInfoData& MonsterInfo)
{
	if (Monsters.Contains(MonsterInfo.ID))
	{
		if (MonsterInfo.HasState(ECharacterStateType::STATE_DIE))
		{
			return;
		}
		Monsters[MonsterInfo.ID]->SetCharacterInfo(MonsterInfo);

		UE_LOG(LogTemp, Warning, TEXT("Update monster [%d]"), MonsterInfo.ID);
	}
}

void UGPObjectManager::DamagedMonster(const FInfoData& MonsterInfo, float Damage)
{
	if (Monsters.Contains(MonsterInfo.ID))
	{
		Monsters[MonsterInfo.ID]->SetCharacterInfo(MonsterInfo);

		FVector SpawnLocation = Monsters[MonsterInfo.ID]->GetActorLocation() + FVector(0, 0, 100);
		FActorSpawnParameters SpawnParams;
		AGPFloatingDamageText* DamageText = World->SpawnActor<AGPFloatingDamageText>(AGPFloatingDamageText::StaticClass(),
			SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		bool isCrt = (MyPlayer->CharacterInfo.GetDamage() != Damage);
		if (DamageText)
		{
			DamageText->SetDamageText(Damage, isCrt);
		}

		UE_LOG(LogTemp, Warning, TEXT("Damaged monster [%d]"), MonsterInfo.ID);
	}
}

void UGPObjectManager::ItemSpawn(uint32 ItemID, uint8 ItemType, FVector Pos)
{
	UE_LOG(LogTemp, Warning, TEXT("ItemSpawn [%d]"), ItemID);

	static const FString DataTablePath = TEXT("/Game/Item/GPItemTable.GPItemTable");
	UDataTable* DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath));
	if (!DataTable)
		return;

	FString ContextString;
	FGPItemStruct* ItemData = DataTable->FindRow<FGPItemStruct>(*FString::FromInt(ItemType), ContextString);

	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemSpawn failed: No matching item found for ID [%d]"), ItemType);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("matching item found for ID [%d]"), ItemType);

	if (!World)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AGPItem* SpawnedItem = World->SpawnActor<AGPItem>(AGPItem::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);

	if (!SpawnedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemSpawn failed: Could not spawn item actor"));
		return;
	}

	SpawnedItem->SetupItem(ItemID, ItemType, 0);
	Items.Add(ItemID, SpawnedItem);

	UE_LOG(LogTemp, Warning, TEXT("ItemSpawn success: Spawned Item ID [%d] at [%s]"), ItemID, *Pos.ToString());
}

void UGPObjectManager::ItemDespawn(uint32 ItemID)
{
	if (Items.Contains(ItemID))
	{
		Items[ItemID]->Destroy();
		Items.Remove(ItemID);
	}
}

void UGPObjectManager::DropItem(uint32 ItemID, uint8 ItemType, FVector Pos)
{
	//Todo : ItemSpawn()과 비슷하지만 둥둥 뜨지 않고 땅바닥에 스폰하도록 
}

void UGPObjectManager::AddInventoryItem(uint32 ItemID, uint8 ItemType)
{
	if (!MyPlayer)
		return;

	UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget();
	if (Inventory)
		Inventory->AddItemToInventory(ItemID, ItemType, 1);
}

void UGPObjectManager::UseInventoryItem(uint32 ItemID)
{
	if (!MyPlayer)
		return;

	UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget();
	if (Inventory)
		Inventory->UseItemFromInventory(ItemID);
}

void UGPObjectManager::EquipItem(int32 PlayerID, uint8 ItemType)
{
	if (!Players.Contains(PlayerID))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipItem Failed: PlayerID [%d] not found"), PlayerID);
		return;
	}

	AGPCharacterPlayer* TargetPlayer = Players[PlayerID];
	if (!TargetPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGPObjectManager::EquipItem , TargetPlayer Not Found"));
		return;
	}

	static const FString DataTablePath = TEXT("/Game/Item/GPItemTable.GPItemTable");
	UDataTable* DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath));
	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGPObjectManager::EquipItem , DataTable Not Found"));
		return;
	}
	FString ContextString;
	FGPItemStruct* ItemData = DataTable->FindRow<FGPItemStruct>(*FString::FromInt(ItemType), ContextString);

	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipItem Failed: No matching item found for ItemType [%d]"), ItemType);
		return;
	}

	if (TargetPlayer->AppearanceHandler)
	{
		TargetPlayer->AppearanceHandler->EquipItemOnCharacter(*ItemData);
	}
	UE_LOG(LogTemp, Warning, TEXT("Player [%d] equipped item: %s"), PlayerID, *ItemData->ItemName.ToString());
}

void UGPObjectManager::UnequipItem(int32 PlayerID, uint8 ItemType)
{
	if (!Players.Contains(PlayerID))
	{
		UE_LOG(LogTemp, Warning, TEXT("UnequipItem Failed: PlayerID [%d] not found"), PlayerID);
		return;
	}

	AGPCharacterPlayer* TargetPlayer = Players[PlayerID];
	if (!TargetPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGPObjectManager::UnequipItem , TargetPlayer Not Found"));
		return;
	}

	static const FString DataTablePath = TEXT("/Game/Item/GPItemTable.GPItemTable");
	UDataTable* DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath));
	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGPObjectManager::UnequipItem , DataTable Not Found"));
		return;
	}

	FString ContextString;
	FGPItemStruct* ItemData = DataTable->FindRow<FGPItemStruct>(*FString::FromInt(ItemType), ContextString);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnequipItem Failed: No matching item found for ItemType [%d]"), ItemType);
		return;
	}

	if (TargetPlayer->AppearanceHandler)
	{
		TargetPlayer->AppearanceHandler->UnequipItemFromCharacter(ItemData->Category);
	}

	UE_LOG(LogTemp, Warning, TEXT("Player [%d] unequipped item: %s"), PlayerID, *ItemData->ItemName.ToString());
}

void UGPObjectManager::ChangeZone(ZoneType newZone, const FVector& RandomPos)
{
	if (!MyPlayer) return;

	auto oldZone = MyPlayer->CharacterInfo.CurrentZone;
	auto GetLevelName = [](ZoneType zone) -> FName {
		switch (zone) {
		case ZoneType::TIP: return "tip";
		case ZoneType::TUK: return "TUK";
		case ZoneType::E:   return "E";
		case ZoneType::GYM: return "gym";
		case ZoneType::INDUSTY: return "industry";
		default: return NAME_None;
		}
		};
	FName NewLevel = GetLevelName(newZone);
	FName OldLevel = GetLevelName(oldZone);
	if (!OldLevel.IsNone()&& !NewLevel.IsNone())
	{
		UGameplayStatics::UnloadStreamLevel(this, OldLevel, FLatentActionInfo(), false);
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("OnZoneLevelLoaded");
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = __LINE__;

		UGameplayStatics::LoadStreamLevel(this, NewLevel, true, true, LatentInfo);

		PendingZone = newZone;
		PendingLocation = RandomPos;
	}
}

void UGPObjectManager::OnZoneLevelLoaded()
{
	MyPlayer->CharacterInfo.SetZone(PendingZone);
	MyPlayer->SetActorLocation(PendingLocation);

	if (MyPlayer->AppearanceHandler)
	{
		MyPlayer->AppearanceHandler->SetupLeaderPose();
	}
}

void UGPObjectManager::RespawnMyPlayer(const FInfoData& info)
{
	//auto PlayerID = info.ID;
	//AGPCharacterPlayer** FoundPlayerPtr = Players.Find(PlayerID);
	//if (!FoundPlayerPtr)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("[Respawn] No player found with ID: %d"), PlayerID);
	//	return;
	//}

	//AGPCharacterPlayer* FoundPlayer = *FoundPlayerPtr;
	//if (FoundPlayer == MyPlayer)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("[Respawn] This is MyPlayer."));

	//	MyPlayer->SetActorHiddenInGame(false);
	//	MyPlayer->SetActorEnableCollision(true);
	//}
}

void UGPObjectManager::ShowShopItems(uint8 Count, const ShopItemInfo* shopitems)
{
	if (!MyPlayer)
		return;

}

void UGPObjectManager::HandleBuyResult(bool bSuccess, DBResultCode Code, uint32 NewGold)
{
	if (!MyPlayer)
		return;

	if (bSuccess)
	{
		MyPlayer->CharacterInfo.Gold = NewGold;
	}
	else
	{
		switch (Code)
		{
		case DBResultCode::NOT_ENOUGH_GOLD:
			UE_LOG(LogTemp, Warning, TEXT("[BuyResult] Not enough gold"));
			break;
		case DBResultCode::ITEM_NOT_FOUND:
			UE_LOG(LogTemp, Warning, TEXT("[BuyResult] Item not found"));
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("[BuyResult] Unknown error"));
			break;
		}
	}

}

void UGPObjectManager::HandleSellResult(bool bSuccess, DBResultCode Code, uint32 NewGold)
{
	if (!MyPlayer)
		return;


}

void UGPObjectManager::OnQuestReward(QuestType Quest, bool bSuccess, uint32 ExpReward, uint32 GoldReward)
{
	UE_LOG(LogTemp, Warning, TEXT("[QuestReward] Called: QuestType = %d, bSuccess = %s, Exp = %d, Gold = %d"),
		static_cast<int32>(Quest),
		bSuccess ? TEXT("true") : TEXT("false"),
		ExpReward,
		GoldReward);

	if (bSuccess)
	{
		switch (Quest)
		{
		case QuestType::NONE:
			UE_LOG(LogTemp, Warning, TEXT("[QuestReward] QuestType::NONE - No reward"));
			break;

		case QuestType::CH1_TALK_TO_STUDENT_A:
			UE_LOG(LogTemp, Warning, TEXT("[QuestReward] CH1_TALK_TO_STUDENT_A reward handling"));
			// NPC 대화 퀘스트 보상 처리
			break;

		case QuestType::CH3_KILL_TINO:
		{
			UE_LOG(LogTemp, Warning, TEXT("[QuestReward] CH3_KILL_TINO reward handling"));

			UGPQuestListEntryWidget* LocalMyPlayerCurrentQuest
				= MyPlayer->UIManager->GetInGameWidget()->QuestListWidget->TinoQuest;

			if (LocalMyPlayerCurrentQuest)
			{
				if (LocalMyPlayerCurrentQuest->EntryType == QuestType::CH3_KILL_TINO)
				{
					UE_LOG(LogTemp, Warning, TEXT("[QuestReward] Updating quest state to Success"));
					LocalMyPlayerCurrentQuest->SetQuestState(TEXT("Success"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[QuestReward] EntryType mismatch. Expected CH3_KILL_TINO, got %d"), static_cast<int32>(LocalMyPlayerCurrentQuest->EntryType));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[QuestReward] TinoQuest widget is nullptr!"));
			}
			break;
		}

		default:
			UE_LOG(LogTemp, Warning, TEXT("[QuestReward] Unhandled QuestType: %d"), static_cast<int32>(Quest));
			break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuestReward] bSuccess is false. No rewards applied."));
	}
}