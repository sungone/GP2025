// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/THCharacterBase.h"
#include "THCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TIGERHUNTER_API ATHCharacterNonPlayer : public ATHCharacterBase
{
	GENERATED_BODY()

public :
	ATHCharacterNonPlayer();

protected :
	void SetDead() override;
	
};
