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

	void SetPlayerInfo(FPlayerInfo& PlayerInfo_);
	void SetPlayerLocationAndRotation(FPlayerInfo& PlayerInfo_);

public:
	FPlayerInfo PlayerInfo;
	FVector PreviousLocation;

// 서버에서 새로 받은 위치 정보들 - 다른 클라이언트들을 움직이기 위해 사용
	FVector NewLocation;
	FRotator NewRotation;
};
