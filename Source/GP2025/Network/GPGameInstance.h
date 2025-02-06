// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../../GP_Server/Source/Common/Proto.h"
#include "GPGameInstance.generated.h"

class AGPCharacterBase;
class AGPCharacterNonPlayer;
/**
 * 
 */

UCLASS()
class GP2025_API UGPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public :
	virtual void Init() override;
	virtual void Shutdown() override;

	void ConnectToServer();
	void DisconnectFromServer();

	void SendPlayerLoginPacket();
	void SendPlayerLogoutPacket();

	void SendPlayerMovePacket();
	void SendPlayerAttackPacket(int32 TargetID = -1);

	void ReceiveData();
	void ProcessPacket();

	void AddPlayer(FInfoData& PlayerInfo, bool isMyPlayer);
	void RemovePlayer(int32 PlayerID);
	void UpdatePlayer(FInfoData& PlayerInfo);

	void AddMonster(FInfoData& MonsterInfo);
	void RemoveMonster(int32 MonsterID);
	void UpdateMonster(FInfoData& MonsterInfo);
	void DamagedMonster(FInfoData& MonsterInfo, float Damage);
public :
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 4000;

public :
	TSubclassOf<AGPCharacterBase> OtherPlayerClass;
	TSubclassOf<AGPCharacterBase> MonsterClass;

	AGPCharacterBase* MyPlayer;
	TMap<int32, AGPCharacterBase*> Players;
	TMap<int32, AGPCharacterBase*> Monsters;
	
	TArray<uint8> RemainingData;
	TQueue<TArray<uint8>, EQueueMode::Mpsc> RecvQueue;
};
