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
	

	// 카메라 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// Sprint Speed 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;

public :
	// Inventory Widget
	UPROPERTY()
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UFUNCTION()
	UGPInventory* GetInventoryWidget();

	// Setting Widget
	UPROPERTY()
	TSubclassOf<UUserWidget> SettingWidgetClass;

	UPROPERTY()
	UUserWidget* SettingWidget;

	// InGame Widget
	UPROPERTY()
	TSubclassOf<UUserWidget> InGameWidgetClass;

	UPROPERTY()
	UUserWidget* InGameWidget;


	// 다른 뷰어 클라이언트들의 위치 동기화를 위해 위치를 계산하는 변수들
public:
	FVector LastLocation;
	float LastRotationYaw;
	FInfoData LastSendPlayerInfo;

public:
	float MovePacketSendTimer = PACKETSENDTIME; // 서버와의 패킷 교환 시간
	float GroundZLocation = 147.7;
	bool isJumpStart = false;
	bool bWasJumping = false;

public :
	// CharacterInfo
	virtual void SetCharacterInfo(FInfoData& CharacterInfo_) override;

public :
	// 아이템 충돌 판정 상태변수 함수들
	bool bInteractItem = false;
	FTimerHandle InteractItemTimerHandle;

public :
	// Gunner Character 에임 위젯 , 함수
	UPROPERTY()
	UUserWidget* GunCrosshairWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GunCrosshairWidgetClass;
	
	bool bIsGunnerCharacter() const;

	float DefaultFOV = 90.f;
	float ZoomedFOV = 60.f;
	float ZoomInterpSpeed = 10.f;
	bool bWantsToZoom = false;

	FVector DefaultCameraOffset = FVector(0.f, 0.f, 100.f);
	FVector ZoomedCameraOffset = FVector(50.f, 30.f, 100.f); 
	float DefaultArmLength = 400.f;
	float ZoomedArmLength = 150.f;

	UFUNCTION(BlueprintCallable, Category = "Zoom")
	bool IsZooming() const { return bWantsToZoom; }


public :
	UFUNCTION(BlueprintCallable, Category = "CharacterType")
	void ChangePlayerType();
};
