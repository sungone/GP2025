// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GPGameInstance.generated.h"

class AGPCharacterPlayer;

UCLASS()
class GP2025_API UGPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	void SetMyPlayer(AGPCharacterPlayer* InMyPlayer);

	void SendPlayerLoginPacket();
	void SendPlayerLogoutPacket();
	void SendPlayerMovePacket();
	void SendPlayerAttackPacket(int32 TargetID = -1);
	void SendPlayerTakeItem(int32 ItemID);

	void ProcessPacket();

	AGPCharacterPlayer* MyPlayer;
private:
	class UGPNetworkManager* NetworkMgr;
};
