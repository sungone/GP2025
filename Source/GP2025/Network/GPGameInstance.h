// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPGameInstance.generated.h"

class AGPCharacterPlayer;
class AGPCharacterMonster;

UCLASS()
class GP2025_API UGPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	void ConnectToServer();
	void DisconnectFromServer();
	
	void SetMyPlayer(AGPCharacterPlayer* InMyPlayer);
	void SendPlayerLoginPacket();
	void SendPlayerLogoutPacket();
	void SendPlayerMovePacket();
	void SendPlayerAttackPacket(int32 TargetID = -1);
	void SendPlayerTakeItem(int32 ItemID);

	void ReceiveData();
	void ProcessPacket();
public:
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 4000;

	AGPCharacterPlayer* MyPlayer;

	TArray<uint8> RemainingData;
	TQueue<TArray<uint8>, EQueueMode::Mpsc> RecvQueue;
};
