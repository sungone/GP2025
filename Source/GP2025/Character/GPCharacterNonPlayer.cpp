// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterNonPlayer.h"
#include "Character/GPCharacterControlData.h"

AGPCharacterNonPlayer::AGPCharacterNonPlayer()
{
	CurrentCharacterControlType = ECharacterType::M_Mouse;
}

void AGPCharacterNonPlayer::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterControl(CurrentCharacterControlType);
}

void AGPCharacterNonPlayer::SetCharacterControlData(const UGPCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);
}

void AGPCharacterNonPlayer::SetCharacterControl(ECharacterType NewCharacterControlType)
{
	Super::SetCharacterControl(NewCharacterControlType);
}
