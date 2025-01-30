// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "GPCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPCharacterNonPlayer : public AGPCharacterBase
{
	GENERATED_BODY()
	
public :
	AGPCharacterNonPlayer();
	virtual void BeginPlay() override;
protected :
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterControlData) override;
	virtual void SetCharacterType(ECharacterType NewCharacterControlType) override;

private :
	ECharacterType CurrentCharacterControlType;
};
