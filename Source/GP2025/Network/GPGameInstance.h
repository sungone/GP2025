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

public:
	UFUNCTION(BlueprintCallable)
	void SaveBoundingBoxData(ULevel* Level);
	UFUNCTION(BlueprintCallable)
	void SaveNavData(bool IsSave);

private:
	UPROPERTY()
	class UGPNetworkManager* NetworkMgr;



 // Android 권한 요청
#if PLATFORM_ANDROID
	void RequestAndroidPermissions();
#endif
};