// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../GP_Server/Proto.h"
#include "GPCharacterBase.generated.h"

UCLASS()
class GP2025_API AGPCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGPCharacterBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetClientInfoFromServer(FPlayerInfo& PlayerInfo_);

public:
	FPlayerInfo PlayerInfo;
};
