#include "Network/GPObjectManager.h"
#include "Character/GPCharacterMyPlayer.h"
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
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GPCharacterControlData.h"
#include "Character/Modules/GPMyplayerSoundManager.h"
#include "ObjectPool/GPFloatingDamageTextPool.h"
#include "ObjectPool/GPItemPool.h"
#include "UI/Friend/GPFriendBox.h"
#include "UI/Friend/GPFriendList.h"
#include "UI/Friend/GPFriendEntry.h"
#include "Network/GPNetworkManager.h"
#include "Inventory/GPEquippedItemSlot.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GPObjectManager.h"
#include "TimerManager.h"

void UGPObjectManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	World = GetWorld();
	OtherPlayerClass = AGPCharacterPlayer::StaticClass();
	MonsterClass = AGPCharacterMonster::StaticClass();

	// Pool Manager 생성
	FloatingDamageTextPool = NewObject<UGPFloatingDamageTextPool>(this);
	FloatingDamageTextPool->Initialize(World, 20);

	ItemPool = NewObject<UGPItemPool>(this);
	ItemPool->Initialize(World, 50); // 풀 초기 크기 50, 네 상황에 맞춰 조절 가능

	static const FString DataTablePath = TEXT("/Game/Item/GPItemTable.GPItemTable");
	ItemDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath));


}

void UGPObjectManager::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("Deinitializing Object Manager..."));

	if (ItemDataTable)
	{
		ItemDataTable = nullptr;
		UE_LOG(LogTemp, Log, TEXT("[ObjectManager] ItemDataTable cache cleared"));
	}

	for (auto& ItemPair : Items)
	{
		TWeakObjectPtr<AGPItem> ItemPtr = ItemPair.Value;
		if (ItemPtr.IsValid())
		{
			AGPItem* Item = ItemPtr.Get();
			if (IsValid(Item))
			{
				UE_LOG(LogTemp, Log, TEXT("Destroying Item: ID [%d]"), ItemPair.Key);
				Item->Destroy();
			}
		}
	}
	Items.Empty();

	for (auto& PlayerPair : Players)
	{
		TWeakObjectPtr<AGPCharacterPlayer> PlayerPtr = PlayerPair.Value;
		if (PlayerPtr.IsValid())
		{
			AGPCharacterPlayer* Player = PlayerPtr.Get();
			if (IsValid(Player))
			{
				UE_LOG(LogTemp, Log, TEXT("Destroying Player: ID [%d]"), PlayerPair.Key);
				Player->Destroy();
			}
		}
	}
	Players.Empty();

	for (auto& MonsterPair : Monsters)
	{
		TWeakObjectPtr<AGPCharacterMonster> MonsterPtr = MonsterPair.Value;
		if (MonsterPtr.IsValid())
		{
			AGPCharacterMonster* Monster = MonsterPtr.Get();
			if (IsValid(Monster))
			{
				UE_LOG(LogTemp, Log, TEXT("Destroying Monster: ID [%d]"), MonsterPair.Key);
				Monster->Destroy();
			}
		}
	}
	Monsters.Empty();

	if (IsValid(MyPlayer))
	{
		UE_LOG(LogTemp, Log, TEXT("Destroying MyPlayer"));
		MyPlayer->Destroy();
		MyPlayer = nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("Object Manager Deinitialized"));
}

void UGPObjectManager::SetMyPlayer(AGPCharacterMyplayer* InMyPlayer)
{
	MyPlayer = InMyPlayer;
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

	auto Weapon = MyPlayer->CharacterInfo.GetEquippedWeapon();
	if (Weapon != Type::EWeapon::NONE)
		EquipItem(PlayerInfo.ID, (uint8)Weapon);

	auto Helmet = MyPlayer->CharacterInfo.GetEquippedHelmet();
	if (Helmet != Type::EArmor::NONE)
		EquipItem(PlayerInfo.ID, (uint8)Helmet);

	auto Chest = MyPlayer->CharacterInfo.GetEquippedChest();
	if (Chest != Type::EArmor::NONE)
		EquipItem(PlayerInfo.ID, (uint8)Chest);
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
	//Player->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);

	Players.Add(PlayerInfo.ID, Player);

	if (TWeakObjectPtr<AGPCharacterPlayer>* WeakPlayerPtr = Players.Find(PlayerInfo.ID))
	{
		if (WeakPlayerPtr->IsValid())
		{
			auto Weapon = Player->CharacterInfo.GetEquippedWeapon();
			if (Weapon != Type::EWeapon::NONE)
				EquipItem(PlayerInfo.ID, (uint8)Weapon);

			auto Helmet = Player->CharacterInfo.GetEquippedHelmet();
			if (Helmet != Type::EArmor::NONE)
				EquipItem(PlayerInfo.ID, (uint8)Helmet);

			auto Chest = Player->CharacterInfo.GetEquippedChest();
			if (Chest != Type::EArmor::NONE)
				EquipItem(PlayerInfo.ID, (uint8)Chest);
		}
	}
}

void UGPObjectManager::RemovePlayer(int32 PlayerID)
{
	if (TWeakObjectPtr<AGPCharacterPlayer>* WeakPlayerPtr = Players.Find(PlayerID))
	{
		if (WeakPlayerPtr->IsValid())
		{
			auto Weapon = WeakPlayerPtr->Get()->CharacterInfo.GetEquippedWeapon();
			if (Weapon != Type::EWeapon::NONE)
				UnequipItem(PlayerID, (uint8)Weapon);

			auto Helmet = WeakPlayerPtr->Get()->CharacterInfo.GetEquippedHelmet();
			if (Helmet != Type::EArmor::NONE)
				UnequipItem(PlayerID, (uint8)Helmet);

			auto Chest = WeakPlayerPtr->Get()->CharacterInfo.GetEquippedChest();
			if (Chest != Type::EArmor::NONE)
				UnequipItem(PlayerID, (uint8)Chest);

			WeakPlayerPtr->Get()->Destroy();
		}

		Players.Remove(PlayerID);
	}
}

void UGPObjectManager::UpdatePlayer(const FInfoData& PlayerInfo)
{
	if (TWeakObjectPtr<AGPCharacterPlayer>* WeakPlayerPtr = Players.Find(PlayerInfo.ID))
	{
		if (WeakPlayerPtr->IsValid())
		{
			WeakPlayerPtr->Get()->SetCharacterInfo(PlayerInfo);
		}
	}
}

void UGPObjectManager::PlayerUseSkill(int32 PlayerID, ESkillGroup SkillGID)
{
	if (TWeakObjectPtr<AGPCharacterPlayer>* WeakPlayerPtr = Players.Find(PlayerID))
	{
		if (WeakPlayerPtr->IsValid())
		{
			AGPCharacterPlayer* Player = WeakPlayerPtr->Get();

			if (SkillGID == ESkillGroup::HitHard || SkillGID == ESkillGroup::Throwing)
			{
				Player->CombatHandler->PlayQSkillMontage();
			}
			else if (SkillGID == ESkillGroup::Clash || SkillGID == ESkillGroup::FThrowing)
			{
				Player->CombatHandler->PlayESkillMontage();
			}
			else if (SkillGID == ESkillGroup::Whirlwind || SkillGID == ESkillGroup::Anger)
			{
				Player->CombatHandler->PlayRSkillMontage();
			}
		}
	}
}

void UGPObjectManager::DamagedPlayer(const FInfoData& PlayerInfo)
{
	if (TWeakObjectPtr<AGPCharacterPlayer>* WeakPlayerPtr = Players.Find(PlayerInfo.ID))
	{
		if (WeakPlayerPtr->IsValid())
		{
			WeakPlayerPtr->Get()->SetCharacterInfo(PlayerInfo);
		}

		AGPCharacterMyplayer* LocalMyPlayer = Cast<AGPCharacterMyplayer>(WeakPlayerPtr->Get());
		if ((LocalMyPlayer == MyPlayer) && MyPlayer->UIManager)
		{
			MyPlayer->UIManager->GetInGameWidget()->HitByMonsterAnimation();
			// Hit Camera Shake
			MyPlayer->PlayerHittedCameraShake();
		}
	}
}

void UGPObjectManager::HandlePlayerDeath(int32 playerId)
{
	TWeakObjectPtr<AGPCharacterPlayer> WeakPlayer = Players.FindRef(playerId);
	AGPCharacterPlayer* TargetPlayer = WeakPlayer.Get();
	if (!TargetPlayer) return;

	if (TargetPlayer == MyPlayer)
	{
		if (MyPlayer->UIManager)
		{
			MyPlayer->UIManager->ShowDeadScreen();
		}
	}

	TargetPlayer->CombatHandler->HandleDeath();
}

void UGPObjectManager::SkillUnlock(ESkillGroup SkillGID)
{
	if (MyPlayer->EffectHandler)
		MyPlayer->EffectHandler->PlaySkillUnlockEffect();

	if (MyPlayer->UIManager)
		MyPlayer->UIManager->ShowSkillUnlockMessage(SkillGID);
}

void UGPObjectManager::SkillUpgrade(ESkillGroup SkillGID)
{
	if (MyPlayer->UIManager)
		MyPlayer->UIManager->ShowSkillUpgradeMessage(SkillGID);
}

void UGPObjectManager::LevelUp(const FInfoData& PlayerInfo)
{
	if (MyPlayer->EffectHandler)
		MyPlayer->EffectHandler->PlayLevelUpEffect();

	UGPInGameWidget* LocalInGameWidget = MyPlayer->UIManager->GetInGameWidget();
	if (LocalInGameWidget)
	{
		int32 PlayerLevel = PlayerInfo.GetLevel();
		FTimerHandle TimerHandle;
		MyPlayer->GetWorldTimerManager().SetTimer(
			TimerHandle,
			[LocalInGameWidget, PlayerLevel]()
			{
				if (PlayerLevel == 5 || PlayerLevel == 8)
				{
					LocalInGameWidget->ShowLevelUpArrowTemporarily(LocalInGameWidget->LevelUpArrowQ);
				}
				else if (PlayerLevel == 6 || PlayerLevel == 9)
				{
					LocalInGameWidget->ShowLevelUpArrowTemporarily(LocalInGameWidget->LevelUpArrowE);
				}
				else if (PlayerLevel == 7 || PlayerLevel == 10)
				{
					LocalInGameWidget->ShowLevelUpArrowTemporarily(LocalInGameWidget->LevelUpArrowR);
				}
			},
			2.0f,
			false
		);
	}
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
		return;
	}

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
	// UE_LOG(LogTemp, Warning, TEXT("Remove monster [%d]"), MonsterID);

	if (TWeakObjectPtr<AGPCharacterMonster>* WeakMonsterPtr = Monsters.Find(MonsterID))
	{
		if (WeakMonsterPtr->IsValid())
		{
			AGPCharacterMonster* Monster = WeakMonsterPtr->Get();
			Monster->Destroy();
		}
		Monsters.Remove(MonsterID);
	}
}

void UGPObjectManager::HandleMonsterDeath(int32 MonsterID)
{
	if (TWeakObjectPtr<AGPCharacterMonster>* WeakMonsterPtr = Monsters.Find(MonsterID))
	{
		if (WeakMonsterPtr->IsValid())
		{
			AGPCharacterMonster* Monster = WeakMonsterPtr->Get();
			Monster->CombatHandler->HandleDeath();

		}
		Monsters.Remove(MonsterID);
	}
}

void UGPObjectManager::UpdateMonster(const FInfoData& MonsterInfo)
{
	if (TWeakObjectPtr<AGPCharacterMonster>* WeakMonsterPtr = Monsters.Find(MonsterInfo.ID))
	{
		if (WeakMonsterPtr->IsValid())
		{
			AGPCharacterMonster* Monster = WeakMonsterPtr->Get();

			Monster->SetCharacterInfo(MonsterInfo);
			FRotator CurrentRot = Monster->GetActorRotation();
			CurrentRot.Yaw = MonsterInfo.Yaw;
			Monster->SetActorRotation(CurrentRot);

			// UE_LOG(LogTemp, Warning, TEXT("Update monster [%d]"), MonsterInfo.ID);
		}
	}
}

void UGPObjectManager::DamagedMonster(const FInfoData& MonsterInfo, float Damage)
{
	if (TWeakObjectPtr<AGPCharacterMonster>* WeakMonsterPtr = Monsters.Find(MonsterInfo.ID))
	{
		if (WeakMonsterPtr->IsValid())
		{
			AGPCharacterMonster* Monster = WeakMonsterPtr->Get();

			Monster->SetCharacterInfo(MonsterInfo);

			if (Monster->CombatHandler)
			{
				Monster->CombatHandler->PlayMonsterHitMontage();
			}

			FVector SpawnLocation = Monster->GetActorLocation() + FVector(0, 0, 100);
			FActorSpawnParameters SpawnParams;

			bool isCrt = (MyPlayer && MyPlayer->CharacterInfo.GetDamage() * 10 < Damage);
			USoundBase* SoundToPlay = nullptr;

			AGPFloatingDamageText* DamageText = FloatingDamageTextPool->Acquire();
			if (DamageText)
			{
				DamageText->SetActorLocation(SpawnLocation);
				DamageText->SetDamageText(Damage, isCrt);
			}

			UE_LOG(LogTemp, Warning, TEXT("Damaged monster [%d]"), MonsterInfo.ID);


			if (isCrt)
			{
				if (Monster->CriticalEffect)
				{
					USkeletalMeshComponent* Mesh = Monster->GetMesh();
					if (Mesh)
					{
						UNiagaraFunctionLibrary::SpawnSystemAttached(
							Monster->CriticalEffect,
							Mesh,
							FName(TEXT("HitSocket")),
							FVector(0.f, 0.f, 0.f),
							FRotator::ZeroRotator,
							EAttachLocation::SnapToTarget,
							true,
							true,
							ENCPoolMethod::None,
							true
						);
					}

					if (Monster->MonsterCriticalHitSound)
						SoundToPlay = Monster->MonsterCriticalHitSound;
				}
			}
			else
			{
				if (Monster->HitEffect)
				{
					USkeletalMeshComponent* Mesh = Monster->GetMesh();
					if (Mesh)
					{
						UNiagaraFunctionLibrary::SpawnSystemAttached(
							Monster->HitEffect,
							Mesh,
							FName(TEXT("HitSocket")),
							FVector(0.f, 0.f, 0.f),
							FRotator::ZeroRotator,
							EAttachLocation::SnapToTarget,
							true,
							true,
							ENCPoolMethod::None,
							true
						);
					}

					if (Monster->MonsterHitSound)
						SoundToPlay = Monster->MonsterHitSound;
				}
			}

			if (SoundToPlay)
			{
				UGameplayStatics::PlaySoundAtLocation(
					Monster,
					SoundToPlay,
					Monster->GetActorLocation()
				);
			}
		}
	}
}

void UGPObjectManager::PlayEarthQuakeEffect(const FVector& RockPos, bool bDebug)
{
	UWorld* WorldContext = GetWorld();
	if (!WorldContext) return;

	//서버 값 처리 확인용
	{

		const FColor SphereColor = (bDebug) ? FColor::Red : FColor::Yellow;
		const FColor LineColor = FColor::Yellow;

		DrawDebugSphere(
			WorldContext,
			RockPos,
			100.f,
			24,
			SphereColor,
			false,
			1.0f
		);
		DrawDebugLine(
			WorldContext,
			RockPos + FVector(0, 0, 500.f),
			RockPos,
			LineColor,
			false,
			3.0f,
			0,
			1.0f
		);

		if (bDebug) return;
	}

	FTimerDelegate DelayedRockDelegate;
	DelayedRockDelegate.BindLambda([=, this]()
		{
			if (MyPlayer && MyPlayer->EffectHandler)
			{
				MyPlayer->EffectHandler->PlayEarthQuakeRock(RockPos);
				UE_LOG(LogTemp, Log, TEXT("[PlayEarthQuakeEffect] Rock effect triggered after delay."));
			}
		});

	FTimerHandle DelayHandle;
	WorldContext->GetTimerManager().SetTimer(
		DelayHandle,
		DelayedRockDelegate,
		0.5f, // ← 0.5초 딜레이
		false
	);
}

void UGPObjectManager::PlayFlameBreathEffect(const FVector& Origin, const FVector& Dir, float Range, float Angle, bool bDebug)
{
	UWorld* WorldContext = GetWorld();
	if (!WorldContext) return;

	//서버 값 처리 확인용
	{
		const int32 NumSegments = 16;
		const float HalfAngleRad = FMath::DegreesToRadians(Angle / 2.0f);

		FVector ForwardDir = Dir.GetSafeNormal2D();
		float BaseYawRad = FMath::Atan2(ForwardDir.Y, ForwardDir.X);

		const FColor Color = bDebug ? FColor::Orange : FColor::Yellow;

		for (int32 i = 0; i <= NumSegments; ++i)
		{
			float T = (float)i / NumSegments;
			float OffsetAngleRad = -HalfAngleRad + T * (2.0f * HalfAngleRad);
			float FinalYaw = BaseYawRad + OffsetAngleRad;

			FVector Direction = FVector(FMath::Cos(FinalYaw), FMath::Sin(FinalYaw), 0.0f);
			FVector EndPoint = Origin + Direction * Range;

			DrawDebugLine(WorldContext, Origin, EndPoint, Color, false, 2.0f, 0, 1.f);
		}

		FVector CenterDir = FVector(FMath::Cos(BaseYawRad), FMath::Sin(BaseYawRad), 0.0f);
		DrawDebugLine(WorldContext, Origin, Origin + CenterDir * Range, Color, false, 2.0f, 0, 1.0f);
	}
}



void UGPObjectManager::ItemSpawn(uint32 ItemID, uint8 ItemType, FVector Pos)
{
	if (!World || !IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemSpawn] World is invalid"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[ItemSpawn] World is valid"));

	if (!ItemDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[ItemSpawn] ItemDataTable is not loaded"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[ItemSpawn] ItemDataTable is valid"));

	if (Items.Contains(ItemID))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemSpawn] Item with ID [%d] already exists, skipping spawn"), ItemID);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[ItemSpawn] Item ID [%d] is not in Items map"), ItemID);

	FString ContextString;
	FGPItemStruct* ItemData = ItemDataTable->FindRow<FGPItemStruct>(*FString::FromInt(ItemType), ContextString);

	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemSpawn] No matching item found for ItemType [%d]"), ItemType);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[ItemSpawn] Item data found for ItemType [%d]: %s"), ItemType, *ItemData->ItemName.ToString());

	AGPItem* SpawnedItem = ItemPool->Acquire();

	if (!SpawnedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemSpawn] Failed to spawn item actor"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[ItemSpawn] Item actor spawned successfully for ItemID [%d]"), ItemID);

	SpawnedItem->SetActorLocation(Pos);
	SpawnedItem->SetupItem(ItemID, ItemType, 0);
	UE_LOG(LogTemp, Log, TEXT("[ItemSpawn] Item setup completed for ItemID [%d]"), ItemID);

	Items.Add(ItemID, SpawnedItem);
	UE_LOG(LogTemp, Log, TEXT("[ItemSpawn] Item [%d] successfully added to Items map"), ItemID);
}

void UGPObjectManager::ItemPickUp(uint32 ItemID)
{
	if (!World || !IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemDespawn] World is invalid"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[ItemDespawn] World is valid"));

	TWeakObjectPtr<AGPItem>* ItemPtr = Items.Find(ItemID);
	if (!ItemPtr || !ItemPtr->IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemDespawn] Item [%d] not found or already destroyed"), ItemID);
		Items.Remove(ItemID);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[ItemDespawn] Item found in Items map for ItemID [%d]"), ItemID);


	AGPItem* Item = ItemPtr->Get();
	if (IsValid(Item))
	{
		UE_LOG(LogTemp, Log, TEXT("[ItemDespawn] Destroying Item [%d]"), ItemID);
		Item->ReturnToPool();

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [this, ItemID]()
			{
				if (Items.Remove(ItemID) > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("[ItemDespawn] Item [%d] successfully removed from Items map"), ItemID);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[ItemDespawn] Item [%d] was not found in Items map during removal"), ItemID);
				}
			}, 0.1f, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemDespawn] Item [%d] is invalid, removing directly"), ItemID);
		Items.Remove(ItemID);
	}
}
void UGPObjectManager::ItemDespawn(uint32 ItemID)
{
	if (!World || !IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemDespawn] World is invalid"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[ItemDespawn] World is valid"));

	TWeakObjectPtr<AGPItem>* ItemPtr = Items.Find(ItemID);
	if (!ItemPtr || !ItemPtr->IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemDespawn] Item [%d] not found or already destroyed"), ItemID);
		Items.Remove(ItemID);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[ItemDespawn] Item found in Items map for ItemID [%d]"), ItemID);


	AGPItem* Item = ItemPtr->Get();
	if (IsValid(Item))
	{
		UE_LOG(LogTemp, Log, TEXT("[ItemDespawn] Destroying Item [%d]"), ItemID);
		Item->ReturnToPool();

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [this, ItemID]()
			{
				if (Items.Remove(ItemID) > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("[ItemDespawn] Item [%d] successfully removed from Items map"), ItemID);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[ItemDespawn] Item [%d] was not found in Items map during removal"), ItemID);
				}
			}, 3.0f, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemDespawn] Item [%d] is invalid, removing directly"), ItemID);
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
	{
		UE_LOG(LogTemp, Warning, TEXT("[AddInventoryItem] MyPlayer is null"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[AddInventoryItem] MyPlayer is valid. Attempting to add ItemID [%d], ItemType [%d]"), ItemID, ItemType);

	UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget();
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AddInventoryItem] Inventory Widget is null"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[AddInventoryItem] Inventory Widget is valid"));

	// Add item to inventory
	Inventory->AddItemToInventory(ItemID, ItemType, 1);
}

void UGPObjectManager::UseInventoryItem(uint32 ItemID)
{
	if (!MyPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UseInventoryItem] MyPlayer is null"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] MyPlayer is valid. Attempting to use ItemID [%d]"), ItemID);

	UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget();
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UseInventoryItem] Inventory Widget is null"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] Inventory Widget is valid"));

	// Use the item
	Inventory->UseItemFromInventory(ItemID);
}

void UGPObjectManager::EquipItem(int32 PlayerID, uint8 ItemType)
{
	UE_LOG(LogTemp, Log, TEXT("[EquipItem] Attempting to equip ItemType [%d] for PlayerID [%d]"), ItemType, PlayerID);

	TWeakObjectPtr<AGPCharacterPlayer>* PlayerPtr = Players.Find(PlayerID);
	if (!PlayerPtr || !PlayerPtr->IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipItem] PlayerID [%d] not found or invalid"), PlayerID);
		return;
	}
	AGPCharacterPlayer* TargetPlayer = PlayerPtr->Get();
	UE_LOG(LogTemp, Log, TEXT("[EquipItem] Player found: %s"), *TargetPlayer->GetName());

	if (!ItemDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipItem] ItemDataTable is null"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[EquipItem] ItemDataTable is valid"));

	FString ContextString;
	FGPItemStruct* ItemData = ItemDataTable->FindRow<FGPItemStruct>(*FString::FromInt(ItemType), ContextString);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipItem] No matching item found for ItemType [%d]"), ItemType);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[EquipItem] Item found for ItemType [%d]: %s"), ItemType, *ItemData->ItemName.ToString());

	if (TargetPlayer->AppearanceHandler)
	{
		UE_LOG(LogTemp, Log, TEXT("[EquipItem] Equipping item: %s on Player [%d]"), *ItemData->ItemName.ToString(), PlayerID);
		TargetPlayer->AppearanceHandler->EquipItemOnCharacter(*ItemData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipItem] AppearanceHandler is null for Player [%d]"), PlayerID);
	}

	UE_LOG(LogTemp, Log, TEXT("[EquipItem] Player [%d] successfully equipped item: %s"), PlayerID, *ItemData->ItemName.ToString());
}

void UGPObjectManager::UnequipItem(int32 PlayerID, uint8 ItemType)
{
	UE_LOG(LogTemp, Log, TEXT("[UnequipItem] Attempting to unequip ItemType [%d] for PlayerID [%d]"), ItemType, PlayerID);

	TWeakObjectPtr<AGPCharacterPlayer>* PlayerPtr = Players.Find(PlayerID);
	if (!PlayerPtr || !PlayerPtr->IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnequipItem] PlayerID [%d] not found or invalid"), PlayerID);
		return;
	}
	AGPCharacterPlayer* TargetPlayer = PlayerPtr->Get();
	UE_LOG(LogTemp, Log, TEXT("[UnequipItem] Player found: %s"), *TargetPlayer->GetName());

	if (!ItemDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnequipItem] ItemDataTable is null"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[UnequipItem] ItemDataTable is valid"));

	FString ContextString;
	FGPItemStruct* ItemData = ItemDataTable->FindRow<FGPItemStruct>(*FString::FromInt(ItemType), ContextString);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnequipItem] No matching item found for ItemType [%d]"), ItemType);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("[UnequipItem] Item found for ItemType [%d]: %s"), ItemType, *ItemData->ItemName.ToString());

	if (TargetPlayer->AppearanceHandler)
	{
		UE_LOG(LogTemp, Log, TEXT("[UnequipItem] Unequipping item of category: %d for Player [%d]"), ItemData->Category, PlayerID);
		TargetPlayer->AppearanceHandler->UnequipItemFromCharacter(ItemData->Category);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnequipItem] AppearanceHandler is null for Player [%d]"), PlayerID);
	}

	UE_LOG(LogTemp, Log, TEXT("[UnequipItem] Player [%d] successfully unequipped item: %s"), PlayerID, *ItemData->ItemName.ToString());
}

void UGPObjectManager::ChangeZone(ZoneType oldZone, ZoneType newZone, const FVector& RandomPos)
{
	if (IsChangingZone())
		return;

	SetChangeingZone(true);

	if (!MyPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed Changing Zone... Player is nullptr"));
		SetChangeingZone(false);
		return;
	}

	if (oldZone == newZone && oldZone == ZoneType::TUK)
	{
		MyPlayer->SetActorLocation(RandomPos);
		SetChangeingZone(false);
		return;
	}

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

	FName OldLevel = GetLevelName(oldZone);
	PendingLevelName = GetLevelName(newZone);
	PendingZone = newZone;
	PendingLocation = RandomPos;

	UE_LOG(LogTemp, Log, TEXT("Start Changing Zone [%d] From %s to %s"),
		MyPlayer->CharacterInfo.ID,
		*OldLevel.ToString(), *PendingLevelName.ToString());

	ULevelStreaming* StreamLevel = UGameplayStatics::GetStreamingLevel(this, OldLevel);
	if (StreamLevel)
	{
		StreamLevel->OnLevelUnloaded.RemoveAll(this);
		StreamLevel->OnLevelUnloaded.AddDynamic(this, &UGPObjectManager::HandleLevelUnloaded);
		StreamLevel->SetShouldBeLoaded(false);
		StreamLevel->SetShouldBeVisible(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Old level not found."));
		SetChangeingZone(false);
	}
}

void UGPObjectManager::HandleLevelUnloaded()
{
	ULevelStreaming* StreamLevel = UGameplayStatics::GetStreamingLevel(this, PendingLevelName);
	if (StreamLevel)
	{
		StreamLevel->OnLevelLoaded.RemoveAll(this);
		StreamLevel->OnLevelLoaded.AddDynamic(this, &UGPObjectManager::HandleLevelLoaded);

		StreamLevel->SetShouldBeLoaded(true);
		StreamLevel->SetShouldBeVisible(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pending level not found."));
		SetChangeingZone(false);
	}
}

void UGPObjectManager::HandleLevelLoaded()
{
	MyPlayer->CharacterInfo.SetZone(PendingZone);
	MyPlayer->SetActorLocation(PendingLocation);

	if (MyPlayer->AppearanceHandler)
	{
		MyPlayer->AppearanceHandler->SetupLeaderPose();
	}

	UE_LOG(LogTemp, Log, TEXT("End Changing Zone [%d]"), MyPlayer->CharacterInfo.ID);

	SetChangeingZone(false);
	if (MyPlayer && MyPlayer->UIManager)
	{
		auto Widget = MyPlayer->UIManager->GetInGameWidget();
		if (Widget)
			Widget->ShowZoneChangeMessage(PendingZone);
	}
}


void UGPObjectManager::RespawnMyPlayer(const FInfoData& info)
{
	ZoneType oldZone = MyPlayer->CharacterInfo.GetZone();
	ZoneType newZone = info.GetZone();
	const FVector& RandomPos = info.Pos;
	if (MyPlayer)
	{
		MyPlayer->SetActorHiddenInGame(false);
		MyPlayer->SetActorEnableCollision(true);
		MyPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		MyPlayer->GetMesh()->SetVisibility(true, true);
		MyPlayer->GetMesh()->SetHiddenInGame(false);

		if (MyPlayer->SoundManager)
		{
			USoundBase** FoundSound = MyPlayer->SoundManager->LevelBGMSounds.Find(TEXT("TUK"));
			if (FoundSound && *FoundSound)
			{
				MyPlayer->SoundManager->PlayBGM(*FoundSound);
				UE_LOG(LogTemp, Log, TEXT("[Respawn] TUK BGM played on respawn."));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Respawn] TUK BGM not found in LevelBGMSounds."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Respawn] OutsideSound is NULL or SoundManager is NULL."));
		}
	}
	UpdatePlayer(info);
	ChangeZone(oldZone, newZone, RandomPos);
}

void UGPObjectManager::OnQuestStart(QuestType Quest)
{
	UE_LOG(LogTemp, Warning, TEXT("=== [ObjectManager] OnQuestStart called: QuestType = %d ==="), static_cast<uint8>(Quest));

	if (MyPlayer && MyPlayer->UIManager)
	{
		uint8 QuestID = static_cast<uint8>(Quest);
		UE_LOG(LogTemp, Warning, TEXT("=== [ObjectManager] Calling UIManager->AddQuestEntry(%d) ==="), QuestID);
		MyPlayer->UIManager->AddQuestEntry(QuestID, false);


		MyPlayer->UIManager->ShowQuestStartMessage(Quest);
		
	}

	if (Quest == QuestType::TUT_COMPLETE) // 튜토리얼 완료는 바로 클리어
	{
		if (MyPlayer && MyPlayer->NetMgr)
		{
			MyPlayer->NetMgr->SendMyCompleteQuest(QuestType::TUT_COMPLETE);
		}
	}
}

void UGPObjectManager::OnQuestReward(QuestType Quest, bool bSuccess, uint32 ExpReward, uint32 GoldReward)
{
	UE_LOG(LogTemp, Warning, TEXT("[QuestReward] Called: QuestType = %d, bSuccess = %s, Exp = %d, Gold = %d"),
		static_cast<uint8>(Quest),
		bSuccess ? TEXT("true") : TEXT("false"),
		ExpReward,
		GoldReward);

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuestReward] Quest Failed. No reward applied."));
		return;
	}

	if (MyPlayer && MyPlayer->UIManager)
	{
		if (MyPlayer->SoundManager && MyPlayer->SoundManager->QuestClearSound)
		{
			MyPlayer->SoundManager->PlaySFX(MyPlayer->SoundManager->QuestClearSound);
		}

		if (MyPlayer->EffectHandler)
		{
			MyPlayer->EffectHandler->PlayQuestClearEffect();
		}

		uint8 QuestID = static_cast<uint8>(Quest);
		MyPlayer->UIManager->UpdateQuestState(QuestID, true);
	}
}

void UGPObjectManager::AddRequestFriend(const FFriendInfo& Info)
{
	uint32 DBId = Info.DBId;
	if (RequestedFriendSet.Contains(DBId))
		return;

	RequestedFriendSet.Add(DBId);

	if (MyPlayer && MyPlayer->UIManager)
	{
		MyPlayer->UIManager->GetFriendBoxWidget()->AddToRequestedList(Info.DBId, UTF8_TO_TCHAR(Info.GetName()), Info.Level, Info.bAccepted);
		MyPlayer->UIManager->AddFriendSystemMessage(EChatFriendNotifyType::RequestReceived, UTF8_TO_TCHAR(Info.GetName()));
	}
}

void UGPObjectManager::RemoveRequestFriend(uint32 DBId)
{
	if (!RequestedFriendSet.Contains(DBId))
		return;

	RequestedFriendSet.Remove(DBId);
	if (MyPlayer && MyPlayer->UIManager)
	{
		MyPlayer->UIManager->GetFriendBoxWidget()->RemoveFromRequestedList(DBId);
		MyPlayer->UIManager->AddFriendSystemMessage(EChatFriendNotifyType::RequestRejected);
	}
}

void UGPObjectManager::AddFriend(const FFriendInfo& Info)
{
	uint32 DBId = Info.DBId;
	if (FriendMap.Contains(DBId))
		return;

	FriendMap.Add(DBId, Info.GetName());

	if (MyPlayer && MyPlayer->UIManager)
	{
		MyPlayer->UIManager->GetFriendBoxWidget()->AddToFriendList(Info.DBId, UTF8_TO_TCHAR(Info.GetName()), Info.Level, Info.isOnline);
		MyPlayer->UIManager->AddFriendSystemMessage(EChatFriendNotifyType::Accepted, UTF8_TO_TCHAR(Info.GetName()));
	}
}

void UGPObjectManager::RemoveFriend(uint32 DBId)
{
	if (!FriendMap.Contains(DBId))return;
	FriendMap.Remove(DBId);

	if (MyPlayer && MyPlayer->UIManager)
	{
		MyPlayer->UIManager->GetFriendBoxWidget()->RemoveFromFriendList(DBId);
		MyPlayer->UIManager->AddFriendSystemMessage(EChatFriendNotifyType::Removed);

	}
}

void UGPObjectManager::ShowFriendMessage(int32 Result)
{
	if (MyPlayer && MyPlayer->UIManager)
	{
		MyPlayer->UIManager->AddFriendSystemMessage(Result);
	}
}

uint32 UGPObjectManager::GetFriendDBId(FString Name) const
{
	return FriendMap.FindKey(Name) ? *FriendMap.FindKey(Name) : 0;
}
