// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "InputActionValue.h"
#include "GPCharacterPlayer.h"
#include "GPCharacterMyplayer.generated.h"


/**
 *
 */

UCLASS()
class GP2025_API AGPCharacterMyplayer : public AGPCharacterPlayer
{
	GENERATED_BODY()

public:
	AGPCharacterMyplayer();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// <Login>
	UFUNCTION()
	void OnPlayerEnterGame();

	// <Lobby>
	UFUNCTION()
	void OnPlayerEnterLobby();

	// <Network>
	UPROPERTY()
	class UGPNetworkManager* NetMgr;

	// <Input>
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly, Category = "MyPlayer")
	class UGPMyplayerInputHandler* InputHandler;

	// <UI>
	UPROPERTY()
	class UGPMyplayerUIManager* UIManager;
	
	// <Camera>
	UPROPERTY(BlueprintReadOnly, Category = "MyPlayer")
	class UGPMyplayerCameraHandler* CameraHandler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// <Network Sync>
	UPROPERTY()
	class UGPMyplayerNetworkSyncHandler* NetworkSyncHandler;

	// <Skill Cool Down Handler>
	UPROPERTY()
	class UGPSkillCoolDownHandler* SkillCoolDownHandler;

	// <Sound Handler>
	UPROPERTY()
	class UGPMyplayerSoundManager* SoundManager;

	// <Skill Cool UI System>
public :
	float GetSkillCooldownRatio(ESkillGroup SkillGroup);
	void UpdateSkillCooldownBars();
	
	// <CharacterType> 
public :
	UFUNCTION(BlueprintCallable, Category = "CharacterType")
	void ChangePlayerType();
	virtual void SetCharacterType(ECharacterType NewCharacterControlType) override;
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterControlData) override;

	// <CharacterInfo>
public :
	virtual void SetCharacterInfo(const FInfoData& CharacterInfo_) override;
	int32 PrevLevel = 1;


public :
	// Camera Shake
	UFUNCTION(BlueprintImplementableEvent, Category = "CameraShake")
	void GunnerAttackCameraShake();

	UFUNCTION(BlueprintImplementableEvent, Category = "CameraShake")
	void WarriorAttackCameraShake();

	UFUNCTION(BlueprintImplementableEvent, Category = "CameraShake")
	void PlayerHittedCameraShake();
public:
	UFUNCTION(BlueprintCallable)
	void PlayFadeOut();

	UFUNCTION(BlueprintCallable)
	void PlayFadeIn();
public:
	UPROPERTY()
	bool bHasPlayedTinoSequence = false;

	void ShowLobbyUI();
	// Tutorial Timer
	FTimerHandle TutorialWidgetTimerHandle;

public :
	void SetDead(bool bDead);
	void SetGPChannel(const EWorldChannel* Channel) { MyChannel = Channel; }
	const EWorldChannel* MyChannel;
	bool bNewPlayer = true;
};
