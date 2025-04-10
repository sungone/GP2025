#include "Network/GPObjectManager.h"
#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterMonster.h"
#include "Item/GPItemStruct.h"
#include "Item/GPItem.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "UI/GPFloatingDamageText.h"
#include "Inventory/GPInventory.h"
#include "Kismet/GameplayStatics.h"

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

void UGPObjectManager::OnLoginSuccess(FInfoData& PlayerInfo)
{
	if (World == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Invaild World"));
		FGenericPlatformMisc::RequestExit(false);
	}
	//Todo: UI로 머리 위에 출력하자. TCHAR로 변환하면 됨
	FString NickName = FString(UTF8_TO_TCHAR(PlayerInfo.GetName()));

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
	MyPlayer->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	auto Player = Cast<AGPCharacterPlayer>(MyPlayer);
	Players.Add(PlayerInfo.ID, Player);
}


void UGPObjectManager::AddPlayer(FInfoData& PlayerInfo)
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

void UGPObjectManager::UpdatePlayer(FInfoData& PlayerInfo)
{
	auto PlayerID = PlayerInfo.ID;
	if (Players.Contains(PlayerID))
	{
		Players[PlayerID]->SetCharacterInfo(PlayerInfo);
	}
}

void UGPObjectManager::DamagedPlayer(FInfoData& PlayerInfo)
{
	auto PlayerID = PlayerInfo.ID;
	if (Players.Contains(PlayerID))
	{
		Players[PlayerID]->SetCharacterInfo(PlayerInfo);
	}
}

void UGPObjectManager::AddMonster(FInfoData& MonsterInfo)
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
	Monsters.Add(MonsterInfo.ID, Monster);
}

void UGPObjectManager::RemoveMonster(int32 MonsterID)
{
	UE_LOG(LogTemp, Warning, TEXT("Remove monster [%d]"), MonsterID);
	if (Monsters.Contains(MonsterID))
	{
		Monsters[MonsterID]->SetDead();
		Monsters.Remove(MonsterID);
	}
}

void UGPObjectManager::UpdateMonster(FInfoData& MonsterInfo)
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

void UGPObjectManager::DamagedMonster(FInfoData& MonsterInfo, float Damage)
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

	TargetPlayer->AppearanceHandler->EquipItemOnCharacter(*ItemData);

	UE_LOG(LogTemp, Warning, TEXT("Player [%d] equipped item: %s"), PlayerID, *ItemData->ItemName.ToString());
}

void UGPObjectManager::UnequipItem(int32 PlayerID, uint8 ItemType)
{
	//Todo: 다른 플레이어 착용 아이템 업데이트
}
