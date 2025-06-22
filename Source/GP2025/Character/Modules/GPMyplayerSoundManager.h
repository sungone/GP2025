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

	// BGM ����
	void PlayBGM(class USoundBase* Sound, float Volume = 0.7f, bool bLoop = true);
	void StopBGM();
	void PlayLoginBGM();
	void PlayBGMForCurrentLevel(); // �� �̸� ��� �ڵ� ���
	void PlayBGMByLevelName(const FName& LevelName);
	// ȿ����
	void PlaySFX(class USoundBase* Sound, float Volume = 1.f);

private:
	UPROPERTY()
	AGPCharacterMyplayer* Owner;

	UPROPERTY()
	class UAudioComponent* BGMComponent;

	UPROPERTY()
	TMap<FName, class USoundBase*> LevelBGMSounds;

	UPROPERTY()
	USoundBase* LoginSound;

	// ������
	UFUNCTION()
	void HandleLoopBGM();
};
