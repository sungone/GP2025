// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "GPCharacterNPC.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPCharacterNPC : public AGPCharacterBase
{
	GENERATED_BODY()


public:
	AGPCharacterNPC();
	virtual void BeginPlay() override;
protected:
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterControlData) override;
	virtual void SetCharacterType(ECharacterType NewCharacterControlType) override;
	
};
