// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPMyplayerNetworkSyncHandler.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPMyplayerNetworkSyncHandler : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(class AGPCharacterMyplayer* InOwner);
	void Tick(float DeltaTime);

public:
	UPROPERTY()
	AGPCharacterMyplayer* Owner;

	FVector LastLocation;
	float LastRotationYaw;
	FInfoData LastSendPlayerInfo;
	float MovePacketSendTimer = 0.5;
	bool bWasJumping = false;

	float WalkSpeed;
	float SprintSpeed;

	// 유틸리티
	void HandleIdleState();
	void HandleJumpState();
	void HandleRotationChange();
	void HandlePeriodicSend();
};
