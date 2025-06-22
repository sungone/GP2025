// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPMyplayerSoundManager.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPMyplayerSoundManager : public UObject
{
	GENERATED_BODY()
	

public:
	UGPMyplayerSoundManager();
	void Initialize(class AGPCharacterMyplayer* InOwner);

	// 사운드 재생 함수
	void PlayBGM(USoundBase* Sound, float Volume = 0.7f, bool bLoop = true);
	void PlayLoginBGM();     
	void StopBGM();
	void PlaySFX(USoundBase* Sound, float Volume = 1.f);
	void HandleLoopBGM();

private:
	UPROPERTY()
	AGPCharacterMyplayer* Owner;

	UPROPERTY()
	class UAudioComponent* BGMComponent;

	UPROPERTY()
	USoundBase* LoginSound;
};
