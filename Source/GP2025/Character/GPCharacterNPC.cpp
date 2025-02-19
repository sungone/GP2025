// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterNPC.h"
#include "Character/GPCharacterControlData.h"

AGPCharacterNPC::AGPCharacterNPC()
{

}

void AGPCharacterNPC::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterType(CurrentCharacterType);
}

void AGPCharacterNPC::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
	Super::SetCharacterData(CharacterControlData);
}

void AGPCharacterNPC::SetCharacterType(ECharacterType NewCharacterControlType)
{
	Super::SetCharacterType(NewCharacterControlType);
}
