// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "GPCharacterMonster.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPCharacterMonster : public AGPCharacterBase
{
	GENERATED_BODY()

public:
	AGPCharacterMonster();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterControlData) override;
	virtual void SetCharacterType(ECharacterType NewCharacterControlType) override;
	

	UPROPERTY(EditAnywhere, Category = "Visibility")
	float VisibilityDistance = 1000.0f;  
};
