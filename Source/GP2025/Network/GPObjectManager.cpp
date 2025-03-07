#include "Network/GPObjectManager.h"
#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterMonster.h"
#include "Item/GPItemStruct.h"
#include "Item/GPItem.h"
#include "UI/GPFloatingDamageText.h"
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
	MyPlayer = InMyPlayer;
}

void UGPObjectManager::AddPlayer(FInfoData& PlayerInfo, bool isMyPlayer)
{
	if (World == nullptr)
		return;

	FVector SpawnLocation(PlayerInfo.Pos);
	FRotator SpawnRotation(0, PlayerInfo.Yaw, 0);

	if (isMyPlayer)
	{
		if (MyPlayer == nullptr)
			return;

		UE_LOG(LogTemp, Warning, TEXT("Add my player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.Pos.X, PlayerInfo.Pos.Y, PlayerInfo.Pos.Z, PlayerInfo.Yaw);

		MyPlayer->SetCharacterInfo(PlayerInfo);
		MyPlayer->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
		Players.Add(PlayerInfo.ID, MyPlayer);
	}
	else
	{
		AGPCharacterPlayer* Player = nullptr;
		while (Player == nullptr)
		{
			Player = World->SpawnActor<AGPCharacterPlayer>(OtherPlayerClass, SpawnLocation, SpawnRotation);
		}

		UE_LOG(LogTemp, Warning, TEXT("Add other player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.Pos.X, PlayerInfo.Pos.Y, PlayerInfo.Pos.Z, PlayerInfo.Yaw);

		Player->SetCharacterInfo(PlayerInfo);
		Player->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
		Players.Add(PlayerInfo.ID, Player);
	}
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
	if (Players.Contains(PlayerInfo.ID))
	{
		Players[PlayerInfo.ID]->SetCharacterInfo(PlayerInfo);
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

void UGPObjectManager::ItemSpawn(uint32 ItemID, EItem ItemType, FVector Pos)
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

	Pos.Z += 200.f;

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

void UGPObjectManager::DropItem(uint32 ItemID, EItem ItemType, FVector Pos)
{
	//Todo : ItemSpawn()과 비슷하지만 둥둥 뜨지 않고 땅바닥에 스폰하도록 
}

void UGPObjectManager::AddInventoryItem(EItem ItemType, uint32 Quantity)
{
	//Todo: myplayer인벤토리 업데이트
}

void UGPObjectManager::RemoveInventoryItem(EItem ItemType, uint32 Quantity)
{
	//Todo: myplayer인벤토리 업데이트

}
