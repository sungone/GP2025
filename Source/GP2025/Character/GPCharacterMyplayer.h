// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "InputActionValue.h"
#define PACKETSENDTIME 0.5
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

	virtual void SetCharacterType(ECharacterType NewCharacterControlType) override;
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterControlData) override;
public:
	UFUNCTION()
	void OnPlayerLoginSucess();
	class UGPNetworkManager* NetMgr;
public:

	// <Input>
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY()
	class UGPMyplayerInputHandler* InputHandler;

	// <UI>
	UPROPERTY()
	class UGPMyplayerUIManager* UIManager;
	
	// <Camera>
	UPROPERTY()
	class UGPMyplayerCameraHandler* CameraHandler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;


	// Sprint Speed ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;


	// �ٸ� ��� Ŭ���̾�Ʈ���� ��ġ ����ȭ�� ���� ��ġ�� ����ϴ� ������
public:
	FVector LastLocation;
	float LastRotationYaw;
	FInfoData LastSendPlayerInfo;

public:
	float MovePacketSendTimer = PACKETSENDTIME; // �������� ��Ŷ ��ȯ �ð�
	float GroundZLocation = 147.7;
	bool isJumpStart = false;
	bool bWasJumping = false;

public :
	// CharacterInfo
	virtual void SetCharacterInfo(FInfoData& CharacterInfo_) override;

public :
	// ������ �浹 ���� ���º��� �Լ���
	bool bInteractItem = false;
	FTimerHandle InteractItemTimerHandle;

public :

	bool bIsGunnerCharacter() const;

public :
	UFUNCTION(BlueprintCallable, Category = "CharacterType")
	void ChangePlayerType();
};
