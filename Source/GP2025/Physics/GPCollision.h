#pragma once

#include "CoreMinimal.h"

#define CPROFILE_GPTRIGGER TEXT("GPTrigger")
#define CCHANNEL_GPACTION ECC_GameTraceChannel1

#include "DrawDebugHelpers.h"

static void DrawDebugCollisionAndAttackRadius(UWorld* World, const FInfoData& InfoData)
{
	if (!World) return;

	// 1. CollisionRadius
	DrawDebugSphere(
		World,
		InfoData.Pos,
		InfoData.CollisionRadius,
		16,
		FColor::Silver
	);

	// 2. AttackRadius
	DrawDebugSphere(
		World,
		InfoData.Pos,
		InfoData.AttackRadius,
		16,
		FColor::Red
	);

	//3. ForwardVectorArrow
	FRotator Rotation(0.0f, InfoData.Yaw, 0.0f);
	FVector ForwardVector = Rotation.Vector();

	float ArrowLength = 200.0f;

	FVector Start = InfoData.Pos;
	FVector End = Start + (ForwardVector * ArrowLength);
	DrawDebugDirectionalArrow(
		World,
		Start,
		End,
		300.0f,
		FColor::Orange,
		false,
		-1.0f,
		0,
		5.0f
	);
}