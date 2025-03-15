// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GPGameInstance.generated.h"

class AGPCharacterPlayer;

UCLASS()
class GP2025_API UGPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	void ProcessPacket();

public:
	UFUNCTION(BlueprintCallable)
	void SaveMapData(ULevel* Level, const FString& PathName);
private:
	class UGPNetworkManager* NetworkMgr;
};
