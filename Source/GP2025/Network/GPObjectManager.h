// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPObjectManager.generated.h"

class AGPCharacterPlayer;
class AGPCharacterMonster;
class AGPItem;

UCLASS()
class GP2025_API UGPObjectManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void SetMyPlayer(AGPCharacterPlayer* InMyPlayer);
	void AddMyPlayer(const FInfoData& PlayerInfo);
	void AddPlayer(const FInfoData& PlayerInfo);
	void RemovePlayer(int32 PlayerID);
	void UpdatePlayer(const FInfoData& PlayerInfo);
	void DamagedPlayer(const FInfoData& PlayerInfo);
	void HandlePlayerDeath(int32 playerId);

	void PlayerUseSkill(int32 PlayerID, ESkillGroup SkillGID);
	void SkillUnlock(ESkillGroup SkillGID);
	void SkillUpgrade(ESkillGroup SkillGID);
	void LevelUp(const FInfoData& PlayerInfo);

	void AddMonster(const FInfoData& MonsterInfo);
	void RemoveMonster(int32 MonsterID);
	void HandleMonsterDeath(int32 MonsterID);
	void UpdateMonster(const FInfoData& MonsterInfo);
	void DamagedMonster(const FInfoData& MonsterInfo, float Damage);
	void PlayEarthQuakeEffect(const FVector& RockPos, bool bDebug);
	void PlayFlameBreathEffect(const FVector& Origin, const FVector& Dir, float Range, float Angle, bool bDebug);

	void ItemSpawn(uint32 ItemID, uint8 ItemType, FVector Pos);
	void ItemPickUp(uint32 ItemID);
	void ItemDespawn(uint32 ItemID);
	void DropItem(uint32 ItemID, uint8 ItemType, FVector Pos);
	void AddInventoryItem(uint32 ItemID, uint8 ItemType);
	void UseInventoryItem(uint32 ItemID);
	void EquipItem(int32 PlayerID, uint8 ItemType);
	void UnequipItem(int32 PlayerID, uint8 ItemType);

	void ChangeZone(ZoneType OldZone, ZoneType NewZone, const FVector& RandomPos);
	bool IsChangingZone() const { return bChangingZone; }
	void SetChangeingZone(bool value) { bChangingZone = value; }

	UFUNCTION()
	void HandleLevelUnloaded();
	UFUNCTION()
	void HandleLevelLoaded();
	void RespawnMyPlayer(const FInfoData& info);
	
	void OnQuestStart(QuestType Quest);
	void OnQuestReward(QuestType Quest, bool bSuccess, uint32 ExpReward, uint32 GoldReward);


	// Friend
	void AddRequestFriend(const FFriendInfo& FriendInfo);
	void RemoveRequestFriend(uint32 DBId);
	void AddFriend(const FFriendInfo& FriendInfo);
	void RemoveFriend(uint32 DBId);
	void ShowFriendMessage(int32 Result);

private:
	UPROPERTY()
	TSubclassOf<AGPCharacterPlayer> OtherPlayerClass;

	UPROPERTY()
	TSubclassOf<AGPCharacterMonster> MonsterClass;

	UPROPERTY()
	class AGPCharacterMyplayer* MyPlayer;

	TMap<int32, TWeakObjectPtr<AGPCharacterPlayer>> Players;

	TMap<int32, TWeakObjectPtr<AGPCharacterMonster>> Monsters;

	TMap<int32, TWeakObjectPtr<AGPItem>> Items;

	TMap<uint32, FFriendInfo> FriendMap;
	TSet<uint32> RequestedFriendSet;

	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	UDataTable* ItemDataTable;

private:
	UPROPERTY()
	class UGPInventory* InventoryWidget;

	ZoneType PendingZone;
	FVector PendingLocation;
	FName PendingLevelName;

	bool bChangingZone = false;

// Object Pool
public :
	UPROPERTY()
	class UGPFloatingDamageTextPool* FloatingDamageTextPool;

	UPROPERTY()
	class UGPItemPool* ItemPool;
};
