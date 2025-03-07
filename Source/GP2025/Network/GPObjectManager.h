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

	void AddPlayer(FInfoData& PlayerInfo, bool isMyPlayer);
	void RemovePlayer(int32 PlayerID);
	void UpdatePlayer(FInfoData& PlayerInfo);

	void AddMonster(FInfoData& MonsterInfo);
	void RemoveMonster(int32 MonsterID);
	void UpdateMonster(FInfoData& MonsterInfo);
	void DamagedMonster(FInfoData& MonsterInfo, float Damage);

	void ItemSpawn(uint32 ItemID, EItem ItemType, FVector Pos);
	void ItemDespawn(uint32 ItemID);
	void DropItem(uint32 ItemID, EItem ItemType, FVector Pos);
	void AddInventoryItem(EItem ItemType, uint32 Quantity);
	void RemoveInventoryItem(EItem ItemType, uint32 Quantity);
private:
    UPROPERTY()
    TSubclassOf<AGPCharacterPlayer> OtherPlayerClass;

    UPROPERTY()
    TSubclassOf<AGPCharacterMonster> MonsterClass;

	AGPCharacterPlayer* MyPlayer;
	TMap<int32, AGPCharacterPlayer*> Players;
    TMap<int32, AGPCharacterMonster*> Monsters;
    TMap<int32, AGPItem*> Items;

    UWorld* World;
};
