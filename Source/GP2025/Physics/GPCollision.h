#pragma once

#include "CoreMinimal.h"

#define CPROFILE_GPTRIGGER TEXT("GPTrigger")
#define CCHANNEL_GPACTION ECC_GameTraceChannel1

#include "DrawDebugHelpers.h"

static void DrawDebugCollisionAndAttackRadius(UWorld* World, const FInfoData& InfoData)
{
	if (!World) return;

	//// 1. CollisionRadius
	//DrawDebugSphere(
	//	World,
	//	InfoData.Pos,
	//	InfoData.CollisionRadius,
	//	16,
	//	FColor::Silver
	//);

	//// 2. AttackRadius
	//DrawDebugSphere(
	//	World,
	//	InfoData.Pos,
	//	InfoData.AttackRadius,
	//	16,
	//	FColor::Silver
	//);

	//3. ForwardVectorArrow
	{
		FRotator Rotation(0.0f, InfoData.Yaw, 0.0f);
		FVector ForwardVector = Rotation.Vector();

		float ArrowLength = 100.0f;

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

	if (InfoData.CharacterType == (uint8)Type::EPlayer::GUNNER)
	{
		const float Range = InfoData.AttackRadius;
		const float Width = 100.f;//서버랑 하드코딩임

		const float YawRad = FMath::DegreesToRadians(InfoData.Yaw);
		const FVector Forward = FVector(FMath::Cos(YawRad), FMath::Sin(YawRad), 0.f);
		const FVector Right = FVector(-Forward.Y, Forward.X, 0.f);

		const FVector Origin = InfoData.Pos;

		const FVector FrontCenter = Origin + Forward * Range;
		const FVector LeftFront = FrontCenter - Right * (Width * 0.5f);
		const FVector RightFront = FrontCenter + Right * (Width * 0.5f);
		const FVector LeftBack = Origin - Right * (Width * 0.5f);
		const FVector RightBack = Origin + Right * (Width * 0.5f);

		DrawDebugLine(World, LeftBack, LeftFront, FColor::Red, false, -1, 0, 2.0f);
		DrawDebugLine(World, RightBack, RightFront, FColor::Red, false, -1, 0, 2.0f);
		DrawDebugLine(World, LeftBack, RightBack, FColor::Red, false, -1, 0, 2.0f);
		DrawDebugLine(World, LeftFront, RightFront, FColor::Green, false, -1, 0, 4.0f);
	}
	else
	{
		// 4. FOV
		const float Radius = InfoData.AttackRadius;
		const float HalfFOV = InfoData.fovAngle * 0.5f;
		const int32 NumSegments = 64;

		for (int32 i = 0; i < NumSegments; ++i)
		{
			float Angle1 = InfoData.Yaw - HalfFOV + (i * (InfoData.fovAngle / NumSegments));
			float Angle2 = InfoData.Yaw - HalfFOV + ((i + 1) * (InfoData.fovAngle / NumSegments));

			FVector Point1 = InfoData.Pos + FVector(FMath::Cos(FMath::DegreesToRadians(Angle1)), FMath::Sin(FMath::DegreesToRadians(Angle1)), 0.0f) * Radius;
			FVector Point2 = InfoData.Pos + FVector(FMath::Cos(FMath::DegreesToRadians(Angle2)), FMath::Sin(FMath::DegreesToRadians(Angle2)), 0.0f) * Radius;

			DrawDebugLine(World, Point1, Point2, FColor::Green, false, -1.0f, 0, 2.0f);
		}

		FVector LeftEdge = InfoData.Pos + FVector(FMath::Cos(FMath::DegreesToRadians(InfoData.Yaw - HalfFOV)), FMath::Sin(FMath::DegreesToRadians(InfoData.Yaw - HalfFOV)), 0.f) * Radius;
		FVector RightEdge = InfoData.Pos + FVector(FMath::Cos(FMath::DegreesToRadians(InfoData.Yaw + HalfFOV)), FMath::Sin(FMath::DegreesToRadians(InfoData.Yaw + HalfFOV)), 0.f) * Radius;

		DrawDebugLine(World, InfoData.Pos, LeftEdge, FColor::Red, false, -1.0f, 0, 2.0f);
		DrawDebugLine(World, InfoData.Pos, RightEdge, FColor::Red, false, -1.0f, 0, 2.0f);
	}
}
