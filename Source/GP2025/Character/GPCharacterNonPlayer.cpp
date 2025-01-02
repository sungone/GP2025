// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterNonPlayer.h"
#include "Character/GPCharacterControlData.h"

AGPCharacterNonPlayer::AGPCharacterNonPlayer()
{
	CurrentCharacterControlType = ECharacterControlType::Mouse;
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

void AGPCharacterNonPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UGPCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	CurrentCharacterControlType = NewCharacterControlType;
}
