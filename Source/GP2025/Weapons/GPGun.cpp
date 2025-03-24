// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GPGun.h"
#include "GameFramework/Actor.h"
#include "Character/GPCharacterBase.h"
#include "Network/GPNetworkManager.h"
#include "Kismet/GameplayStatics.h"

AGPGun::AGPGun()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGPGun::StartAttack()
{
	// AttackHitCheck();
}

void AGPGun::AttackHitCheck()
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * 5000.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, StartLocation, EndLocation, ECC_Visibility, Params);

	DrawDebugLine(
		GetWorld(),
		StartLocation,
		bHit ? HitResult.ImpactPoint : EndLocation,
		FColor::Red,
		false, 2.0f, 0, 2.0f);

	if (bHit)
	{
		AGPCharacterBase* TargetCharacter = Cast<AGPCharacterBase>(HitResult.GetActor());
		if (TargetCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("ÃÑ °ø°Ý ¼º°ø!"));

			auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
			if (NetworkMgr)
			{
				NetworkMgr->SendPlayerAttackPacket(TargetCharacter->CharacterInfo.ID);
			}
		}
	}
}
