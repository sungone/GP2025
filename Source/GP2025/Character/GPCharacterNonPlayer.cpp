// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterNonPlayer.h"
#include "Character/GPCharacterControlData.h"

AGPCharacterNonPlayer::AGPCharacterNonPlayer()
{
	CurrentCharacterType = ECharacterType::M_MOUSE;
}

void AGPCharacterNonPlayer::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterType(CurrentCharacterType);
}

void AGPCharacterNonPlayer::SetCharacterData(const UGPCharacterControlData* CharacterData)
{
	Super::SetCharacterData(CharacterData);
}

void AGPCharacterNonPlayer::SetCharacterType(ECharacterType NewCharacterType)
{
	Super::SetCharacterType(NewCharacterType);
}
