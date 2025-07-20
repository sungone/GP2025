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
	void PlaySFX(class USoundBase* Sound, float Pitch = 1.f, float Volume = 1.f);

public:
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

public :
	// ȿ����(SFX)
	UPROPERTY()
	USoundBase* ClickSound;

	UPROPERTY()
	USoundBase* GunnerAttackSound;

	UPROPERTY()
	USoundBase* GunnerESkillSound;

	UPROPERTY()
	USoundBase* GunnerPlayerZoomSound;

	UPROPERTY()
	USoundBase* GunnerQSkillSound;

	UPROPERTY()
	USoundBase* GunnerRSkillSound;

	UPROPERTY()
	USoundBase* PlayerJumpSound;

	UPROPERTY()
	USoundBase* PlayerPunchSound;

	UPROPERTY()
	USoundBase* PlayerWalkSound;

	UPROPERTY()
	USoundBase* WarriorAttackSound;

	UPROPERTY()
	USoundBase* WarriorESkillSound;

	UPROPERTY()
	USoundBase* WarriorQSkillSound;

	UPROPERTY()
	USoundBase* WarriorRSkillSound;

	UPROPERTY()
	USoundBase* QuestClearSound;

	UPROPERTY()
	USoundBase* LevelUpSound;

	UPROPERTY()
	USoundBase* MonsterHitSound;

	UPROPERTY()
	USoundBase* TeleportationSound;

	UPROPERTY()
	USoundBase* WarningSound;

	UPROPERTY()
	USoundBase* ShopBuySellSound;

	UPROPERTY()
	USoundBase* PickUpItemSound;

	UPROPERTY()
	USoundBase* PlayerHitSound;

	UPROPERTY()
	USoundBase* PlayerHit;

	UPROPERTY()
	USoundBase* FinalQuestClearSound;
};
