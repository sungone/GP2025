// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GPSword.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "Physics/GPCollision.h"

AGPSword::AGPSword()
{
	// ÄÝ¸®Àü Ä¸½¶ »ý¼º
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(WeaponMesh);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AGPSword::AttackHitCheck()
{
	AGPCharacterMyplayer* PlayerCharacter = Cast<AGPCharacterMyplayer>(GetOwner());
	if (!PlayerCharacter) return;

	/*FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = PlayerCharacter->CharacterInfo.AttackRange;
	const float AttackRadius = PlayerCharacter->CharacterInfo.AttackRadius;

	const FVector Start = GetActorLocation() + GetActorForwardVector() * PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool bHitDetected = GetWorld()->SweepSingleByChannel(
		OutHitResult, Start, End, FQuat::Identity, CCHANNEL_GPACTION,
		FCollisionShape::MakeSphere(AttackRadius), Params);

	if (bHitDetected && IsValid(OutHitResult.GetActor()))
	{
		AGPCharacterBase* TargetCharacter = Cast<AGPCharacterBase>(OutHitResult.GetActor());
		if (IsValid(TargetCharacter))
		{
			const FInfoData& TargetInfo = TargetCharacter->CharacterInfo;

			const FVector TargetLocation = TargetCharacter->GetActorLocation();
			const float TargetCollisionRadius = TargetInfo.CollisionRadius;
			const float TargetHalfHeight = TargetCollisionRadius;

#if ENABLE_DRAW_DEBUG
			DrawDebugCapsule(GetWorld(), TargetLocation, TargetHalfHeight, TargetCollisionRadius,
				FQuat::Identity, FColor::Yellow, false, 5.f);
#endif
			auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
			if (NetworkMgr)
			{
				NetworkMgr->SendPlayerAttackPacket(TargetCharacter->CharacterInfo.ID);
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	const FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	const float CapsuleHalfHeight = AttackRange * 0.5f;
	const FColor DrawColor = FColor::Red;
	if (bHitDetected)
	{
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius,
			FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
			DrawColor, false, 5.f);
	}
#endif*/

}
