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
#include "Character/Modules/GPMyplayerInputHandler.h"
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
#include "TimerManager.h"
#include "Sequence/GPSequenceManager.h"
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

void UGPObjectManager::PlayerAttack(int32 PlayerID, FVector PlayerPos, float PlayerYaw)
{
	if (TWeakObjectPtr<AGPCharacterPlayer>* WeakPlayerPtr = Players.Find(PlayerID))
	{
		if (WeakPlayerPtr->IsValid())
		{
			AGPCharacterPlayer* Player = WeakPlayerPtr->Get();
			if (Player->CombatHandler)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Player %d Attack!"), PlayerID));

				Player->CharacterInfo.SetLocation(PlayerPos);
				Player->CharacterInfo.SetYaw(PlayerYaw);
				Player->CharacterInfo.AddState(STATE_AUTOATTACK);
				Player->HandleAutoAttackState();
				Player->CharacterInfo.RemoveState(STATE_AUTOATTACK);
			}
		}
	}
}

void UGPObjectManager::PlayerUseSkillStart(int32 PlayerID, ESkillGroup SkillGID, float PlayerYaw, FVector PlayerPos)
{
	if (TWeakObjectPtr<AGPCharacterPlayer>* WeakPlayerPtr = Players.Find(PlayerID))
	{
		if (!WeakPlayerPtr->IsValid()) return;
		AGPCharacterPlayer* Player = WeakPlayerPtr->Get();
		if (!Player->CombatHandler) return;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Player %d UseSkill!"), PlayerID));
		Player->CharacterInfo.SetLocation(PlayerPos);
		Player->CharacterInfo.SetYaw(PlayerYaw);
		switch (SkillGID)
		{
		case ESkillGroup::HitHard:
		case ESkillGroup::Throwing:
			Player->CharacterInfo.AddState(STATE_SKILL_Q);
			Player->HandleQSkillState();
			break;
		case ESkillGroup::Clash:
		case ESkillGroup::FThrowing:
			Player->CharacterInfo.AddState(STATE_SKILL_E);
			Player->HandleESkillState();
			break;
		case ESkillGroup::Whirlwind:
		case ESkillGroup::Anger:
			Player->CharacterInfo.AddState(STATE_SKILL_R);
			Player->HandleRSkillState();
			break;
		default:
			break;
		}
	}
}

void UGPObjectManager::PlayerUseSkillEnd(int32 PlayerID)
{
	if (TWeakObjectPtr<AGPCharacterPlayer>* WeakPlayerPtr = Players.Find(PlayerID))
	{
		if (!WeakPlayerPtr->IsValid()) return;
		AGPCharacterPlayer* Player = WeakPlayerPtr->Get();
		Player->CharacterInfo.RemoveState(STATE_SKILL_Q);
		Player->CharacterInfo.RemoveState(STATE_SKILL_E);
		Player->CharacterInfo.RemoveState(STATE_SKILL_R);
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
		}

		if ((LocalMyPlayer == MyPlayer) && MyPlayer->EffectHandler)
		{
			MyPlayer->EffectHandler->PlayPlayerHitEffect();
		}

		if ((LocalMyPlayer == MyPlayer) && MyPlayer->SoundManager)
		{
			MyPlayer->SoundManager->PlaySFX(MyPlayer->SoundManager->PlayerHit);
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

		MyPlayer->SetDead(true);

		if (MyPlayer->SoundManager)
		{
			if (MyPlayer->bIsGunnerCharacter())
			{
				MyPlayer->SoundManager->PlaySFX(MyPlayer->SoundManager->GunnerDeadSound);
			}
			else
			{
				MyPlayer->SoundManager->PlaySFX(MyPlayer->SoundManager->WarriorDeadSound);
			}
		}
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
	if (Monster->CharacterInfo.CharacterType == static_cast<uint8>(Type::EMonster::TINO))
	{
		Tino = Monster;
		Monster->SetActorHiddenInGame(true);
		Monster->SetActorEnableCollision(false);
	}
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
			if (Monster->CharacterInfo.CharacterType == static_cast<uint8>(Type::EMonster::TINO))
			{
				Tino = nullptr;
			}
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

			if (Monster->MonsterDeadSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					Monster,
					Monster->MonsterDeadSound,
					Monster->GetActorLocation()
				);
			}

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

			bool isCrt = IsCritical(Damage);
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

			if (SoundToPlay && Monster->CharacterInfo.GetHp() > 0)
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

bool UGPObjectManager::IsCritical(float Damage)
{
	if (!MyPlayer) return false;
	float PlayerDamage = MyPlayer->CharacterInfo.GetDamage();
	float PlayerCrtValue = MyPlayer->CharacterInfo.GetCrtValue();

	if (Damage >= (PlayerDamage * PlayerCrtValue))
	{
		return true;
	}
	else
	{
		return false;
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

	if (MyPlayer && MyPlayer->EffectHandler)
	{
		MyPlayer->EffectHandler->PlayEarthQuakeRock(RockPos);
		UE_LOG(LogTemp, Log, TEXT("[PlayEarthQuakeEffect] Rock effect started immediately"));
	}
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

	// 스킬 이펙트 처리
	for (UGPItemSlot* Slot : Inventory->EatableSlots)
	{
		if (!Slot)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UseInventoryItem] Found null slot in EatableSlots"));
			continue;
		}

		const TArray<int32>& UniqueIDs = Slot->SlotData.ItemUniqueIDs;
		UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] Checking Slot: %s"), *Slot->GetName());

		if (UniqueIDs.Contains(ItemID))
		{
			UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] Match found in Slot: %s with ItemID: %d"), *Slot->GetName(), ItemID);

			FDataTableRowHandle TypeID = Slot->SlotData.ItemID;
			FName Row = TypeID.RowName;
			UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] Item RowName: %s"), *Row.ToString());

			// 회복 아이템 (RowName: 20, 21, 22 , 23)
			if (Row == FName("20") || Row == FName("21") || Row == FName("22") || Row == FName("23"))
			{
				UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] Detected Heal item with RowName: %s"), *Row.ToString());
				if (MyPlayer->EffectHandler)
				{
					MyPlayer->EffectHandler->PlayHealEffect();
					UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] Heal effect played"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[UseInventoryItem] EffectHandler is null (Heal)"));
				}
			}
			// 공격력 버프 아이템 (RowName: 24, 25)
			else if (Row == FName("24") || Row == FName("25"))
			{
				UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] Detected Attack Buff item with RowName: %s"), *Row.ToString());
				if (MyPlayer->EffectHandler)
				{
					MyPlayer->EffectHandler->PlayAttackBuffEffect();
					UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] Attack buff effect played"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[UseInventoryItem] EffectHandler is null (Buff)"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[UseInventoryItem] Unknown item RowName: %s - No effect played"), *Row.ToString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[UseInventoryItem] Slot %s does not contain ItemID %d"), *Slot->GetName(), ItemID);
		}
	}
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

void UGPObjectManager::SoundWhenBuy(ResultCode Code)
{
	if (MyPlayer->SoundManager)
	{
		switch (Code)
		{
		case ResultCode::SUCCESS:
			MyPlayer->SoundManager->PlaySFX(MyPlayer->SoundManager->ShopBuySellSound);
			break;
		case ResultCode::NOT_ENOUGH_GOLD:
			MyPlayer->SoundManager->PlaySFX(MyPlayer->SoundManager->WarningSound);
			break;
		case ResultCode::ITEM_NOT_FOUND:
			break;
		default:
			break;
		}
	}
}

void UGPObjectManager::ChangeChannel(const FVector& RandomPos)
{
	UE_LOG(LogTemp, Log, TEXT("[ChangeChannel] Start changing channel"));

	for (auto& PlayerPair : Players)
	{
		TWeakObjectPtr<AGPCharacterPlayer> PlayerPtr = PlayerPair.Value;
		if (PlayerPtr.IsValid() && PlayerPtr.Get() != MyPlayer)
		{
			PlayerPtr->Destroy();
		}
	}
	Players.Empty();
	UE_LOG(LogTemp, Log, TEXT("[ChangeChannel] All other players removed"));

	for (auto& MonsterPair : Monsters)
	{
		TWeakObjectPtr<AGPCharacterMonster> MonsterPtr = MonsterPair.Value;
		if (MonsterPtr.IsValid())
		{
			MonsterPtr->Destroy();
		}
	}
	Monsters.Empty();
	UE_LOG(LogTemp, Log, TEXT("[ChangeChannel] All monsters removed"));

	for (auto& ItemPair : Items)
	{
		TWeakObjectPtr<AGPItem> ItemPtr = ItemPair.Value;
		if (ItemPtr.IsValid())
		{
			ItemPtr->ReturnToPool();
		}
	}
	Items.Empty();
	UE_LOG(LogTemp, Log, TEXT("[ChangeChannel] All items removed"));

	if (MyPlayer)
	{
		ZoneType CurZone = MyPlayer->CharacterInfo.GetZone();
		ChangeZone(CurZone, START_ZONE, RandomPos);

		MyPlayer->PlayFadeIn();
		if (MyPlayer->UIManager)
		{
			MyPlayer->UIManager->GetInGameWidget()->ShowGameMessage(FText::FromString(TEXT("채널이 변경되었습니다.")), 2.f);
		}
		auto Player = Cast<AGPCharacterPlayer>(MyPlayer);
		Players.Add(MyPlayer->CharacterInfo.ID, Player);
	}
}


void UGPObjectManager::ChangeZone(ZoneType oldZone, ZoneType newZone, const FVector& RandomPos)
{
	if (IsChangingZone())
		return;

	SetChangeingZone(true);

	if (!IsValid(MyPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("[ChangeZone] Failed: MyPlayer is nullptr."));
		SetChangeingZone(false);
		return;
	}

	if (oldZone == newZone && oldZone == ZoneType::TUK)
	{
		if (RandomPos != FVector::ZeroVector)
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

	if (IsValid(MyPlayer->SoundManager))
	{
		MyPlayer->SoundManager->StopBGM();
		if (RandomPos != FVector::ZeroVector)
			MyPlayer->SoundManager->PlayBGMByLevelName(PendingLevelName);
	}

	UE_LOG(LogTemp, Log, TEXT("Start Changing Zone [%d] From %s to %s"),
		MyPlayer->CharacterInfo.ID,
		*OldLevel.ToString(), *PendingLevelName.ToString());

	if (!IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Error, TEXT("[ChangeZone] GetWorld() is invalid."));
		SetChangeingZone(false);
		return;
	}

	ULevelStreaming* StreamLevel = UGameplayStatics::GetStreamingLevel(this, OldLevel);
	if (StreamLevel)
	{
		if (!StreamLevel->IsLevelLoaded())
		{
			HandleLevelUnloaded();
			return;
		}

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

FRotator UGPObjectManager::GetDefaultZoneRotation(ZoneType Zone)
{
	switch (Zone)
	{
	case ZoneType::TIP:     return FRotator(0.f, 90.f, 0.f);
	case ZoneType::TUK:     return FRotator(0.f, -90.f, 0.f);
	case ZoneType::E:       return FRotator(0.f, 80.f, 0.f);
	case ZoneType::GYM:     return FRotator(0.f, -30.f, 0.f);
	case ZoneType::INDUSTY: return FRotator(0.f, 90.f, 0.f);;
	default:                return FRotator::ZeroRotator;
	}
}

void UGPObjectManager::HandleLevelUnloaded()
{
	if (!IsValid(this))
	{
		return;
	}

	if (!IsValid(MyPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ObjectManager] MyPlayer is null during HandleLevelUnloaded."));
		SetChangeingZone(false);
		return;
	}

	ULevelStreaming* StreamLevel = UGameplayStatics::GetStreamingLevel(this, PendingLevelName);
	if (StreamLevel)
	{
		if (!StreamLevel->IsLevelLoaded())
		{
			StreamLevel->OnLevelLoaded.RemoveAll(this);
			StreamLevel->OnLevelLoaded.AddDynamic(this, &UGPObjectManager::HandleLevelLoaded);

			StreamLevel->SetShouldBeLoaded(true);
			StreamLevel->SetShouldBeVisible(true);
		}
		else
		{
			HandleLevelLoaded();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pending level not found."));
		SetChangeingZone(false);
	}
}

void UGPObjectManager::HandleLevelLoaded()
{
	if (!IsValid(MyPlayer))
	{
		SetChangeingZone(false);
		return;
	}

	MyPlayer->CharacterInfo.SetZone(PendingZone);
	if (PendingLocation != FVector::ZeroVector)
	{
		MyPlayer->SetActorLocation(PendingLocation);
		FRotator NewRotation = GetDefaultZoneRotation(PendingZone);
		MyPlayer->SetActorRotation(NewRotation);
		if (MyPlayer->AppearanceHandler)
		{
			MyPlayer->AppearanceHandler->SetupLeaderPose();
		}

		if (MyPlayer->UIManager)
		{
			MyPlayer->PlayFadeIn();
			auto Widget = MyPlayer->UIManager->GetInGameWidget();

			if (Widget)
			{
				Widget->ShowZoneChangeMessage(PendingZone);
			}
		}
	}

	SetChangeingZone(false);
	UE_LOG(LogTemp, Log, TEXT("End Changing Zone [%d]"), MyPlayer->CharacterInfo.ID);
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

	FTimerHandle RespawnDelayHandle;
	MyPlayer->GetWorldTimerManager().SetTimer(
		RespawnDelayHandle,
		FTimerDelegate::CreateLambda([this]()
			{
				if (MyPlayer)
				{
					MyPlayer->SetDead(false);
				}
			}),
		2.f,
		false
	);
}

void UGPObjectManager::ShowTutorialStartQuest()
{
	if (!MyPlayer) return;

	auto QuestType = MyPlayer->CharacterInfo.CurrentQuest.QuestType;
	if (QuestType == QuestType::TUT_START
		|| QuestType == QuestType::NONE)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this]()
				{
					if (!MyPlayer) return;

					if (MyPlayer->UIManager)
					{
						MyPlayer->UIManager->PlayTutorialQuestWidget();
					}
				}),
			1.0f,
			false);
	}
}

void UGPObjectManager::OnQuestStart(QuestType Quest)
{
	UE_LOG(LogTemp, Warning, TEXT("=== [ObjectManager] OnQuestStart called: QuestType = %d ==="), static_cast<uint8>(Quest));

	if (!MyPlayer) return;
	MyPlayer->CharacterInfo.CurrentQuest = QuestStatus(Quest, EQuestStatus::InProgress);

	if (Quest == QuestType::CH4_KILL_TINO)
	{
		PlayTinoIntro();
	}

	if (MyPlayer->UIManager)
	{
		uint8 QuestID = static_cast<uint8>(Quest);
		UE_LOG(LogTemp, Warning, TEXT("=== [ObjectManager] Calling UIManager->AddQuestEntry(%d) ==="), QuestID);
		MyPlayer->UIManager->AddQuestEntry(QuestID, false);
		MyPlayer->UIManager->ShowQuestStartMessage(Quest);
	}

	if (Quest == QuestType::TUT_COMPLETE)
	{
		if (MyPlayer->NetMgr)
		{
			FTimerHandle RespawnDelayHandle;
			MyPlayer->GetWorldTimerManager().SetTimer(
				RespawnDelayHandle,
				FTimerDelegate::CreateLambda([this]()
					{
						if (MyPlayer)
						{
							MyPlayer->NetMgr->SendMyCompleteQuest();
						}
					}),
				5.f,
				false
			);

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
		if (MyPlayer->SoundManager && Quest == QuestType::CH4_KILL_TINO)
		{
			MyPlayer->SoundManager->PlaySFX(MyPlayer->SoundManager->FinalQuestClearSound);
			MyPlayer->UIManager->ShowQuestStartMessage(QuestType::CH4_GAME_CLEAR);
		}
		else if (MyPlayer->SoundManager && MyPlayer->SoundManager->QuestClearSound)
		{
			MyPlayer->SoundManager->PlaySFX(MyPlayer->SoundManager->QuestClearSound);
		}

		if (MyPlayer->EffectHandler)
		{
			MyPlayer->EffectHandler->PlayQuestClearEffect();
		}

		uint8 QuestID = static_cast<uint8>(Quest);
		MyPlayer->UIManager->UpdateQuestState(QuestID, true);
		MyPlayer->UpdateUIInfo();
	}
}

void UGPObjectManager::PlayWorldIntro()
{
	UWorld* MyWorld = GetWorld();
	if (!MyWorld) return;
	ChangeZone(ZoneType::TUK, ZoneType::E, FVector::ZeroVector);

	UGPGameInstance* GI = Cast<UGPGameInstance>(UGameplayStatics::GetGameInstance(MyWorld));
	if (!GI) return;

	UGPSequenceManager* SeqMgr = GI->GetSequenceManager();
	if (!SeqMgr) return;
	SeqMgr->OnSequenceFinishedDelegate.BindUObject(this, &UGPObjectManager::OnWorldIntroFinished);
	SeqMgr->PlaySequenceByName(this, TEXT("WorldIntro"));
}

void UGPObjectManager::PlayTinoIntro()
{
	UWorld* MyWorld = GetWorld();
	if (!MyWorld) return;

	if (MyPlayer->InputHandler)
	{
		MyPlayer->InputHandler->SetInputEnabled(false);
	}

	UGPGameInstance* GI = Cast<UGPGameInstance>(UGameplayStatics::GetGameInstance(MyWorld));
	if (!GI) return;

	UGPSequenceManager* SeqMgr = GI->GetSequenceManager();
	if (!SeqMgr) return;
	SeqMgr->OnSequenceFinishedDelegate.BindUObject(this, &UGPObjectManager::OnTinoIntroFinished);
	SeqMgr->PlaySequenceByName(this, TEXT("TinoIntro"));
}

void UGPObjectManager::StopLoginSound()
{
	if (MyPlayer->SoundManager)
		MyPlayer->SoundManager->StopLoginBGM();
}

void UGPObjectManager::OnWorldIntroFinished()
{
	ChangeZone(ZoneType::E, ZoneType::TUK, FVector::ZeroVector);
	MyPlayer->ShowLobbyUI();
}

void UGPObjectManager::OnTinoIntroFinished()
{
	if (Tino)
	{
		Tino->SetActorHiddenInGame(false);
		Tino->SetActorEnableCollision(true);
	}

	if (MyPlayer->InputHandler)
	{
		MyPlayer->InputHandler->SetInputEnabled(true);
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
		auto* FriendWidgetPtr = MyPlayer->UIManager->GetFriendBoxWidget();
		if (FriendWidgetPtr)
		{
			FriendWidgetPtr->AddToRequestedList(Info.DBId, UTF8_TO_TCHAR(Info.GetName()), Info.Level, Info.bAccepted);
			MyPlayer->UIManager->AddFriendSystemMessage(EChatFriendNotifyType::RequestReceived, UTF8_TO_TCHAR(Info.GetName()));
		}
	}
}

void UGPObjectManager::RemoveRequestFriend(uint32 DBId)
{
	if (!RequestedFriendSet.Contains(DBId))
		return;

	RequestedFriendSet.Remove(DBId);
	if (MyPlayer && MyPlayer->UIManager)
	{
		auto* FriendWidgetPtr = MyPlayer->UIManager->GetFriendBoxWidget();
		if (FriendWidgetPtr)
		{
			FriendWidgetPtr->RemoveFromRequestedList(DBId);
			MyPlayer->UIManager->AddFriendSystemMessage(EChatFriendNotifyType::RequestRejected);
		}
	}
}

void UGPObjectManager::AddFriend(const FFriendInfo& Info)
{
	uint32 DBId = Info.DBId;
	if (FriendMap.Contains(DBId))
		return;

	FriendMap.Add(DBId, UTF8_TO_TCHAR(Info.GetName()));

	if (MyPlayer && MyPlayer->UIManager)
	{
		auto* FriendWidgetPtr = MyPlayer->UIManager->GetFriendBoxWidget();
		if (FriendWidgetPtr)
		{
			FriendWidgetPtr->AddToFriendList(Info.DBId, UTF8_TO_TCHAR(Info.GetName()), Info.Level, Info.isOnline);
			MyPlayer->UIManager->AddFriendSystemMessage(EChatFriendNotifyType::Accepted, UTF8_TO_TCHAR(Info.GetName()));
		}
	}
}

void UGPObjectManager::RemoveFriend(uint32 DBId)
{
	if (!FriendMap.Contains(DBId))return;
	FriendMap.Remove(DBId);

	if (MyPlayer && MyPlayer->UIManager)
	{
		auto* FriendWidgetPtr = MyPlayer->UIManager->GetFriendBoxWidget();
		if (FriendWidgetPtr)
		{
			FriendWidgetPtr->RemoveFromFriendList(DBId);
			MyPlayer->UIManager->AddFriendSystemMessage(EChatFriendNotifyType::Removed);
		}
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
