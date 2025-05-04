// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerNetworkSyncHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GPMyplayerNetworkSyncHandler.h"


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

	const float CurrentSpeed = Owner->GetVelocity().Size();
	Owner->CharacterInfo.Stats.Speed = CurrentSpeed;

	HandleIdleState();
	Owner->UpdateGroundZLocation();
	HandleJumpState();
	HandleRotationChange();
	HandlePeriodicSend();
}

// 1. �̵� ���η� IDLE ���� �Ǵ�
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

// 2. ���� �� ó��
void UGPMyplayerNetworkSyncHandler::HandleJumpState()
{
	// ���� ���� ó��
	if (Owner->NetworkSyncHandler->isJumpStart && !bWasJumping)
	{
		Owner->NetworkSyncHandler->isJumpStart = false;
		bWasJumping = true;

		Owner->NetMgr->SendMyMovePacket();
		LastSendPlayerInfo = Owner->CharacterInfo;

		UE_LOG(LogTemp, Log, TEXT("[UGPMyplayerNetworkSyncHandler::HandleJumpState] Jump Start"));
		return;
	}

	// ���� ó��
	if (bWasJumping && Owner->GetCharacterMovement()->IsMovingOnGround())
	{
		bWasJumping = false;
		UE_LOG(LogTemp, Log, TEXT("[UGPMyplayerNetworkSyncHandler::HandleJumpState] Jump End"));
	}

	// [UGPMyplayerNetworkSyncHandler::HandleJumpState] ���� ü�� ���� ���� ó��
	const float AirThreshold = 10.f;
	if (Owner->CharacterInfo.HasState(STATE_IDLE) &&
		!Owner->CharacterInfo.HasState(STATE_JUMP) &&
		(LastSendPlayerInfo.Pos.Z - Owner->Ground_ZLocation) > AirThreshold)
	{
		Owner->CharacterInfo.Pos.Z = Owner->Ground_ZLocation;

		Owner->NetMgr->SendMyMovePacket();
		LastSendPlayerInfo = Owner->CharacterInfo;

		UE_LOG(LogTemp, Log, TEXT("[UGPMyplayerNetworkSyncHandler::HandleJumpState] Air Fix Issue"));
	}
}

// 3. ȸ�� ���� �� ó��
void UGPMyplayerNetworkSyncHandler::HandleRotationChange()
{
	const float YawThreshold = 10.f;

	float CurrentYaw = Owner->GetActorRotation().Yaw;
	float YawDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentYaw, LastRotationYaw));

	bool bYawChanged = YawDifference > YawThreshold;
	LastRotationYaw = CurrentYaw;

	if (bYawChanged && Owner->CharacterInfo.HasState(STATE_IDLE))
	{
		Owner->NetMgr->SendMyMovePacket();
		LastSendPlayerInfo = Owner->CharacterInfo;

		UE_LOG(LogTemp, Log, TEXT("Send Packet: Yaw Changed in Idle"));
	}
}

// 4. �ֱ��� ��Ŷ ����
void UGPMyplayerNetworkSyncHandler::HandlePeriodicSend()
{
	const float NotMovedThreshold = 2.f;
	float DistanceMoved = FVector::DistSquared(Owner->GetActorLocation(), LastLocation);

	if (MovePacketSendTimer <= 0 || (Owner->CharacterInfo.HasState(STATE_IDLE) && DistanceMoved >= NotMovedThreshold))
	{
		MovePacketSendTimer = PAKCETSENDTIME;

		if (!Owner->CharacterInfo.HasState(STATE_IDLE) || DistanceMoved >= NotMovedThreshold)
		{
			Owner->NetMgr->SendMyMovePacket();
			LastSendPlayerInfo = Owner->CharacterInfo;
			UE_LOG(LogTemp, Log, TEXT("Send Packet: Periodic or Idle Move"));
		}
	}
}
