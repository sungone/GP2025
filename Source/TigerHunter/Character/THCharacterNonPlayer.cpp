// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/THCharacterNonPlayer.h"

ATHCharacterNonPlayer::ATHCharacterNonPlayer()
{
}

void ATHCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), DeadEventDelayTime, false);
}
