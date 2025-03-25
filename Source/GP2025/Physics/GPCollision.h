#pragma once

#include "CoreMinimal.h"

#define CPROFILE_GPTRIGGER TEXT("GPTrigger")
#define CCHANNEL_GPACTION ECC_GameTraceChannel1

#include "DrawDebugHelpers.h"

static void DrawDebugCollisionAndAttackRadius(UWorld* World, const FInfoData& InfoData)
{
	if (!World) return;

	// 1. CollisionRadius 그리기 (파란색)
	DrawDebugSphere(
		World,
		InfoData.Pos,
		InfoData.CollisionRadius,
		16,                     // 세그먼트 개수 (클수록 더 부드럽게 보임)
		FColor::Blue,
		false,
		0.0f,                   // 지속 시간 (5초 동안 표시)
		0,
		1.0f                    // 선의 두께
	);

	// 2. AttackRadius 그리기 (빨간색)
	DrawDebugSphere(
		World,
		InfoData.Pos,
		InfoData.AttackRadius,
		16,
		FColor::Red,
		false,
		0.0f,
		0,
		1.0f
	);
}