// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPNetworkManager.generated.h"

class AGPCharacterPlayer;
class AGPCharacterMonster;

UCLASS()
class GP2025_API UGPNetworkManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
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

private:
	class FSocket* Socket;
	FString IpAddress = SERVER_IP;
	int16 Port = SERVER_PORT;

	AGPCharacterPlayer* MyPlayer;

	TArray<uint8> RemainingData;
	TQueue<TArray<uint8>, EQueueMode::Mpsc> RecvQueue;
};
