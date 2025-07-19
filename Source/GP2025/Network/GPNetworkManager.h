// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPNetworkManager.generated.h"

class AGPCharacterPlayer;
class AGPCharacterMyplayer;
class AGPCharacterMonster;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnReceiveChat, uint8, Channel, const FString&, Sender, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBuyItemResult, bool, bSuccess, uint32, CurrentGold, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSellItemResult, bool, bSuccess, uint32, CurrentGold, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserAuthFailed, FString, Message);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterLobby);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterGame);

UCLASS()
class GP2025_API UGPNetworkManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnUserAuthFailed OnUserAuthFailed;

	UPROPERTY(BlueprintAssignable)
	FOnEnterLobby OnEnterLobby;

	UPROPERTY(BlueprintAssignable)
	FOnEnterGame OnEnterGame;

	UPROPERTY(BlueprintAssignable)
	FOnReceiveChat OnReceiveChat;

	UPROPERTY(BlueprintAssignable)
	FOnBuyItemResult OnBuyItemResult;

	UPROPERTY(BlueprintAssignable)
	FOnSellItemResult OnSellItemResult;
public:
	bool ConnectToServer();
	void DisconnectFromServer();
	void ProcessPacket();

	void SetMyPlayer(AGPCharacterMyplayer* InMyPlayer);
public:
	void HandleUserAuthFailure(ResultCode ResCode);
	void HandleBuyItemResult(bool bSuccess, uint32 CurrentGold, ResultCode ResCode);
	void HandleSellItemResult(bool bSuccess, uint32 CurrentGold, ResultCode ResCode);
	
public:
	UFUNCTION(BlueprintCallable)
	void SendMySelectCharacter(uint8 PlayerType);
	UFUNCTION(BlueprintCallable)
	void SendMyLoginPacket(const FString& AccountID, const FString& AccountPW);
	void SendMySignUpPacket(const FString& AccountID, const FString& AccountPW, const FString& NickName);
	void SendMyLogoutPacket();
	void SendMyEnterGamePacket();
	void SendMyEnterGamePacket(Type::EPlayer PlayerType);

	void SendMyZoneChangePacket(ZoneType zone);
	void SendMyRespawnPacket(ZoneType zone);

	void SendMyMovePacket();
	void SendMyAttackPacket(float PlayerYaw, FVector PlayerPos);
	void SendMyStartAiming(float PlayerYaw, FVector PlayerPos);
	void SendMyStopAiming();
	void SendMyTakeItem(int32 ItemID);
	void SendMyDropItem(int32 ItemID);
	void SendMyUseItem(int32 ItemID);
	void SendMyEquipItem(int32 ItemID);
	void SendMyUnequipItem(int32 ItemID);
	void SendMyUseSkill(ESkillGroup SkillGID, float PlayerYaw, FVector PlayerPos);

	void SendMyShopBuyItem(uint8 ItemTypeID, int32 Quantity);
	void SendMyShopSellItem(int32 ItemID);
	void SendMyRequestQuest(QuestType quest);
	void SendMyCompleteQuest(QuestType quest);
	void SendMyRejectQuest(QuestType quest);

	void SendMyChatMessage(const FString& Message, EChatChannel Channel);
	void SendMyWhisperMessage(const FString& TargetName, const FString& Message);
	void SendMyRemoveStatePacket(uint32 State);

	void SendMyFriendRequest(const FString& TargetNickName);
	void SendMyFriendAccept(uint32 RequesterUserID);
	void SendMyFriendReject(uint32 RequesterUserID);
	void SendMyFriendRemove(uint32 TargetUserID);

private:
	void SendPacket(uint8* Buf, int32 Size);
	void ReceiveData();
private:
	class FSocket* Socket;
	FString IpAddress = SERVER_IP;
	//FString IpAddress = "211.188.61.31";
	int16 Port = SERVER_PORT;

	UPROPERTY()
	AGPCharacterMyplayer* MyPlayer;

	TArray<uint8> RemainingData;
	TQueue<TArray<uint8>, EQueueMode::Mpsc> RecvQueue;
};
