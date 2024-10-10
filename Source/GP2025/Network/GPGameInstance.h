// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GPGameInstance.generated.h"

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
	void SendPlayerMovePacket(FVector Position, FRotator Rotation, bool IsJumping);

public :
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 4000;
	TSharedPtr<class GPNetworkThread> NetworkThread;

public :
	int32 PlayerID;
	FORCEINLINE int32 GetPlayerID() const { return PlayerID; }
	FORCEINLINE void SetPlayerID(int32 ID) { PlayerID = ID; }

// All PlayerID Managed
public :

	// ���� ������ ������ �÷��̾��� ������ �����ϴ� Map
	TMap<class AGPCharacterPlayer* , int32> PlayerArr;
	AGPCharacterPlayer* FindPlayerByID(int32 ID);
	void AddPlayer(AGPCharacterPlayer* NewPlayer , int32 ID);
	void RemovePlayer(int32 ID);

};
