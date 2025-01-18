// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../../GP_Server/Proto.h"
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
	void sendPlayerAttackPacket();
	
	void ReceiveData();
	void ProcessPacket();

	void AddPlayer(FPlayerInfo& PlayerInfo, bool isMyPlayer);
	void RemovePlayer(int32 PlayerID);
	void UpdatePlayer(FPlayerInfo& PlayerInfo);

	void SpawnMonster(FMonsterInfo& MonsterInfo);

public :
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 4000;
	TSharedPtr<class GPRecvThread> RecvThread;

public :
	TSubclassOf<AGPCharacterBase> OtherPlayerClass;
	TSubclassOf<AGPCharacterBase> MonsterClass;

	AGPCharacterBase* MyPlayer;
	TMap<int32, AGPCharacterBase*> Players;
	TMap<int32, AGPCharacterBase*> Monsters;
	
	TArray<uint8> RemainingData;
	TQueue<TArray<uint8>, EQueueMode::Mpsc> RecvQueue;
};
