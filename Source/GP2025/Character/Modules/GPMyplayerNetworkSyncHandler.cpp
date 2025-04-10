// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerNetworkSyncHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "GameFramework/CharacterMovementComponent.h"


void UGPMyplayerNetworkSyncHandler::Initialize(AGPCharacterMyplayer* InOwner)
{
	Owner = InOwner;

	if (Owner)
	{
		LastLocation = Owner->GetActorLocation();
		LastRotationYaw = Owner->GetActorRotation().Yaw;
		LastSendPlayerInfo = Owner->CharacterInfo;
	}
}

void UGPMyplayerNetworkSyncHandler::Tick(float DeltaTime)
{
	if (!Owner || !Owner->NetMgr) return;

	MovePacketSendTimer -= DeltaTime;

	Owner->CharacterInfo.SetLocation(Owner->GetActorLocation().X, Owner->GetActorLocation().Y, Owner->GetActorLocation().Z);
	Owner->CharacterInfo.Yaw = Owner->GetActorRotation().Yaw;
	Owner->CharacterInfo.Stats.Speed = Owner->GetVelocity().Size();

	HandleIdleState();
	HandleJumpState();
	HandleRotationChange();
	HandlePeriodicSend();
}

// 1. 이동 여부로 IDLE 상태 판단
void UGPMyplayerNetworkSyncHandler::HandleIdleState()
{
	const float NotMovedThreshold = 2.f;

	float DistanceMoved = FVector::DistSquared(Owner->GetActorLocation(), LastLocation);
	LastLocation = Owner->GetActorLocation();

	if (DistanceMoved >= NotMovedThreshold)
	{
		Owner->CharacterInfo.RemoveState(STATE_IDLE);
	}
	else
	{
		Owner->CharacterInfo.AddState(STATE_IDLE);
	}
}

// 2. 점프 시 처리
void UGPMyplayerNetworkSyncHandler::HandleJumpState()
{
	if (Owner->isJumpStart && !bWasJumping)
	{
		Owner->isJumpStart = false;
		bWasJumping = true;

		Owner->NetMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = Owner->CharacterInfo;

		UE_LOG(LogTemp, Log, TEXT("Send Packet: Jump"));
		return;
	}

	if (Owner->GetCharacterMovement()->IsMovingOnGround())
	{
		bWasJumping = false;
	}

	const float AirThreshold = 10.f;
	if (Owner->CharacterInfo.HasState(STATE_IDLE) &&
		!Owner->CharacterInfo.HasState(STATE_JUMP) &&
		(LastSendPlayerInfo.Pos.Z - Owner->GroundZLocation) > AirThreshold)
	{
		Owner->CharacterInfo.Pos.Z = Owner->GroundZLocation;
		Owner->CharacterInfo.Stats.Speed = LastSendPlayerInfo.HasState(STATE_RUN) ? 
			Owner->NetworkSyncHandler->SprintSpeed : Owner->NetworkSyncHandler->WalkSpeed;

		Owner->NetMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = Owner->CharacterInfo;

		UE_LOG(LogTemp, Log, TEXT("Send Packet: Air Fix"));
	}
}

// 3. 회전 변경 시 처리
void UGPMyplayerNetworkSyncHandler::HandleRotationChange()
{
	const float YawThreshold = 10.f;

	float CurrentYaw = Owner->GetActorRotation().Yaw;
	float YawDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentYaw, LastRotationYaw));

	bool bYawChanged = YawDifference > YawThreshold;
	LastRotationYaw = CurrentYaw;

	if (bYawChanged && Owner->CharacterInfo.HasState(STATE_IDLE))
	{
		Owner->NetMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = Owner->CharacterInfo;

		UE_LOG(LogTemp, Log, TEXT("Send Packet: Yaw Changed in Idle"));
	}
}

// 4. 주기적 패킷 전송
void UGPMyplayerNetworkSyncHandler::HandlePeriodicSend()
{
	const float NotMovedThreshold = 2.f;
	float DistanceMoved = FVector::DistSquared(Owner->GetActorLocation(), LastLocation);

	if (MovePacketSendTimer <= 0 || (Owner->CharacterInfo.HasState(STATE_IDLE) && DistanceMoved >= NotMovedThreshold))
	{
		MovePacketSendTimer = PACKETSENDTIME;

		if (!Owner->CharacterInfo.HasState(STATE_IDLE) || DistanceMoved >= NotMovedThreshold)
		{
			Owner->NetMgr->SendPlayerMovePacket();
			LastSendPlayerInfo = Owner->CharacterInfo;

			UE_LOG(LogTemp, Log, TEXT("Send Packet: Periodic or Idle Move"));
		}
	}
}