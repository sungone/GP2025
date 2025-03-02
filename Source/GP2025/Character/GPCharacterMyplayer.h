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
DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UGPItemData*);

USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()
	FTakeItemDelegateWrapper() {}
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) : ItemDelegate(InItemDelegate) {}
	FOnTakeItemDelegate ItemDelegate;
};


UCLASS()
class GP2025_API AGPCharacterMyplayer : public AGPCharacterPlayer, public IGPCharacterItemInterface
{
	GENERATED_BODY()

public:
	AGPCharacterMyplayer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetCharacterType(ECharacterType NewCharacterControlType) override;
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterControlData) override;

	// Input 함수
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	virtual void StopJumping() override;
	void StartSprinting();
	void StopSprinting();
	void AutoAttack();
	void ToggleInventory();
	void OpenInventory();
	void CloseInventory();
	void ResetInventoryToggle();
	bool bInventoryToggled = false;

	// 카메라 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// Input 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AutoAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InventoryAction;

	// Sprint Speed 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;

	// Inventory Widget
	UPROPERTY()
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY()
	UUserWidget* InventoryWidget;

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


	// Item Section
protected:
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;

	virtual void TakeItem(class UGPItemData* InItemData) override;
	virtual void DrinkPotion(class UGPItemData* InItemData);
	virtual void EquipChest(class UGPItemData* InItemData);
	virtual void EquipHelmet(class UGPItemData* InItemData);
	virtual void AddExp(class UGPItemData* InItemData);

	// Money Section
public :
	// Sprint Speed 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Money")
	int32 MoneyAmount;
};
