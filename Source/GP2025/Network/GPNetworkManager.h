// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPNetworkManager.generated.h"

class AGPCharacterPlayer;
class AGPCharacterMonster;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginFailed, FString, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOnEnterGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOnEnterLobby);

UCLASS()
class GP2025_API UGPNetworkManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnLoginFailed OnLoginFailed;

	UPROPERTY(BlueprintAssignable)
	FOnOnEnterLobby OnEnterLobby;

	UPROPERTY(BlueprintAssignable)
	FOnOnEnterGame OnEnterGame;
public:
	void ConnectToServer();
	void DisconnectFromServer();
	void ProcessPacket();

	void SetMyPlayer(AGPCharacterPlayer* InMyPlayer);
public:
	void PrintFailMessege(DBResultCode ResultCode);

	UFUNCTION(BlueprintCallable)
	void SendPlayerSelectCharacter(uint8 PlayerType);
	UFUNCTION(BlueprintCallable)
	void SendPlayerLoginPacket(const FString& AccountID, const FString& AccountPW);
	void SendPlayerSignUpPacket(const FString& AccountID, const FString& AccountPW, const FString& NickName);
	void SendPlayerLogoutPacket();

	void SendPlayerMovePacket();
	void SendPlayerAttackPacket(float PlayerYaw);
	void SendPlayerStartAiming(float PlayerYaw);
	void SendPlayerStopAiming();
	void SendPlayerTakeItem(int32 ItemID);
	void SendPlayerDropItem(int32 ItemID);
	void SendPlayerUseItem(int32 ItemID);
	void SendPlayerEquipItem(int32 ItemID);
	void SendPlayerUnequipItem(int32 ItemID);

	void SendPlayerUseSkill(ESkillGroup SkillGID, float PlayerYaw);
private:
	void SendPacket(uint8* Buf, int32 Size);
	void ReceiveData();
private:
	class FSocket* Socket;
	FString IpAddress = SERVER_IP;
	int16 Port = SERVER_PORT;

	AGPCharacterPlayer* MyPlayer;

	TArray<uint8> RemainingData;
	TQueue<TArray<uint8>, EQueueMode::Mpsc> RecvQueue;
};
