#include "Network/GPObjectManager.h"
#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterMonster.h"
#include "Item/GPItemStruct.h"
#include "Item/GPItem.h"
#include "Character/GPCharacterMyplayer.h"
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
	MyPlayer = InMyPlayer;
}

void UGPObjectManager::Login(FInfoData& PlayerInfo)
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
	MyPlayer->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);

}

void UGPObjectManager::AddPlayer(FInfoData& PlayerInfo)
{
	if (World == nullptr)
		return;

	FVector SpawnLocation(PlayerInfo.Pos);
	FRotator SpawnRotation(0, PlayerInfo.Yaw, 0);

	AGPCharacterPlayer* Player = nullptr;
	while (Player == nullptr)
	{
		//이렇게 처리한 이유가 뭐지? 이 방식이 알맞나? 고민해보자
		Player = World->SpawnActor<AGPCharacterPlayer>(OtherPlayerClass, SpawnLocation, SpawnRotation);
	}

	UE_LOG(LogTemp, Warning, TEXT("Add other player [%d] (%f,%f,%f)(%f)"),
		PlayerInfo.ID, PlayerInfo.Pos.X, PlayerInfo.Pos.Y, PlayerInfo.Pos.Z, PlayerInfo.Yaw);

	Player->SetCharacterInfo(PlayerInfo);
	Player->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	OtherPlayers.Add(PlayerInfo.ID, Player);
}

void UGPObjectManager::RemovePlayer(int32 PlayerID)
{
	if (OtherPlayers.Contains(PlayerID))
	{
		OtherPlayers[PlayerID]->Destroy();
		OtherPlayers.Remove(PlayerID);
	}
}

void UGPObjectManager::UpdatePlayer(FInfoData& PlayerInfo)
{
	if (OtherPlayers.Contains(PlayerInfo.ID))
	{
		OtherPlayers[PlayerInfo.ID]->SetCharacterInfo(PlayerInfo);
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

	Pos.Z -= 70.f; // 아이템 위치 약간 내리기

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
	UE_LOG(LogTemp, Warning, TEXT("AddInventoryItem - ItemID: %d | ItemType: %d"), ItemID, ItemType);

	AGPCharacterMyplayer* LocalMyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!LocalMyPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("No Player..."));
		return;
	}

	if (!LocalMyPlayer->InventoryWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget not found!"));
		return;
	}

	UGPInventory* LocalInventoryWidget = Cast<UGPInventory>(LocalMyPlayer->InventoryWidget);
	if (!LocalInventoryWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Cast InventoryWidget to UGPInventory"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Attempting to Add Item - ItemType: %d to Inventory"), ItemType);

	LocalInventoryWidget->AddItemToInventory(ItemType, 1);

	UE_LOG(LogTemp, Warning, TEXT("Item Successfully Added to Inventory!"));
}

void UGPObjectManager::RemoveInventoryItem(uint32 ItemID)
{
	//Todo: myplayer인벤토리 업데이트

}

void UGPObjectManager::EquipItem(int32 PlayerID, uint8 ItemType)
{
	//Todo: 다른 플레이어 착용 아이템 업데이트
}

void UGPObjectManager::UnequipItem(int32 PlayerID, uint8 ItemType)
{
	//Todo: 다른 플레이어 착용 아이템 업데이트
}
