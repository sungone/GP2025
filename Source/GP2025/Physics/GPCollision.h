#pragma once

#include "CoreMinimal.h"

#define CPROFILE_GPTRIGGER TEXT("GPTrigger")
#define CCHANNEL_GPACTION ECC_GameTraceChannel1

#include "DrawDebugHelpers.h"

static void DrawDebugCollisionAndAttackRadius(UWorld* World, const FInfoData& InfoData)
{
	if (!World) return;

	// 1. CollisionRadius �׸��� (�Ķ���)
	DrawDebugSphere(
		World,
		InfoData.Pos,
		InfoData.CollisionRadius,
		16,                     // ���׸�Ʈ ���� (Ŭ���� �� �ε巴�� ����)
		FColor::Blue,
		false,
		0.0f,                   // ���� �ð� (5�� ���� ǥ��)
		0,
		1.0f                    // ���� �β�
	);

	// 2. AttackRadius �׸��� (������)
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