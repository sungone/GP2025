// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerInputHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/Modules/GPMyplayerNetworkSyncHandler.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Character/Modules/GPCharacterCombatHandler.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Skill/GPSkillCoolDownHandler.h"
#include "Player/GPPlayerController.h"
#include "Inventory/GPInventory.h"
#include "Blueprint/UserWidget.h"
#include "Network/GPNetworkManager.h"
#include "Network/GPGameInstance.h"
#include "TimerManager.h"


UGPMyplayerInputHandler::UGPMyplayerInputHandler()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/PlayerInput/IMC_PlayerIMC.IMC_PlayerIMC'"));
	if (InputMappingContextRef.Object)
	{
		DefaultMappingContext = InputMappingContextRef.Object;
	}

	auto LoadAction = [](const FString& Path, TObjectPtr<UInputAction>& Target)
		{
			ConstructorHelpers::FObjectFinder<UInputAction> ActionFinder(*Path);
			if (ActionFinder.Succeeded())
			{
				Target = ActionFinder.Object;
			}
		};

	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Move.IA_Move'"), MoveAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Look.IA_Look'"), LookAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Jump.IA_Jump'"), JumpAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Sprint.IA_Sprint'"), SprintAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_AutoAttack.IA_AutoAttack'"), AutoAttackAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Inventory.IA_Inventory'"), InventoryAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_ESC.IA_ESC'"), SettingAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_TakeItem.IA_TakeItem'"), TakeItemAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Zoom.IA_Zoom'"), ZoomAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_SkillQ.IA_SkillQ'"), SkillQAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_SkillE.IA_SkillE'"), SkillEAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_SkillR.IA_SkillR'"), SkillRAction);
}

void UGPMyplayerInputHandler::Initialize(AGPCharacterMyplayer* InOwner, UEnhancedInputComponent* InputComponent)
{
	if (!InOwner || !InputComponent) return;
	Owner = InOwner;

	SetupInputBindings(InputComponent);
}

void UGPMyplayerInputHandler::SetupInputBindings(UEnhancedInputComponent* EnhancedInput)
{
	if (!Owner || !EnhancedInput) return;

	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::Jump);
	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::StopJumping);

	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::Move);
	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::Look);

	EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::StartSprinting);
	EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::StopSprinting);

	EnhancedInput->BindAction(AutoAttackAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::AutoAttack);

	EnhancedInput->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::ToggleInventory);
	EnhancedInput->BindAction(InventoryAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::ResetInventoryToggle);

	EnhancedInput->BindAction(SettingAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::OpenSettingWidget);
	EnhancedInput->BindAction(TakeItemAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::TakeItem);

	EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::StartAiming);
	EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::StopAiming);

	EnhancedInput->BindAction(SkillQAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::UseSkillQ);
	EnhancedInput->BindAction(SkillEAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::UseSkillE);
	EnhancedInput->BindAction(SkillRAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::UseSkillR);
}


void UGPMyplayerInputHandler::Move(const FInputActionValue& Value)
{
	if (!Owner || !Owner->Controller) return;

	if (Owner->CameraHandler->bWantsToZoom)
		return;

	FVector2D Input = Value.Get<FVector2D>();
	const FRotator ControlRotation = Owner->GetControlRotation();
	const FRotator YawRotation(0, ControlRotation.Yaw, 0);

	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	Owner->AddMovementInput(Forward, Input.X);
	Owner->AddMovementInput(Right, Input.Y);
}

void UGPMyplayerInputHandler::Look(const FInputActionValue& Value)
{
	if (!Owner) return;

	FVector2D LookAxis = Value.Get<FVector2D>();
	Owner->AddControllerYawInput(LookAxis.X);
	Owner->AddControllerPitchInput(LookAxis.Y);
}

void UGPMyplayerInputHandler::Jump()
{
	if (!Owner || !bCanJump) return;

	Owner->Jump();
	bCanJump = false;

	Owner->NetworkSyncHandler->isJumpStart = true;
	Owner->CharacterInfo.RemoveState(STATE_IDLE);
	Owner->CharacterInfo.AddState(STATE_JUMP);
	Owner->AppearanceHandler->SetupLeaderPose();

	Owner->GetWorldTimerManager().SetTimer(
		JumpCooldownTimerHandle,
		[this]()
		{
			bCanJump = true;
		},
		1.f,
		false
	);
}

void UGPMyplayerInputHandler::StopJumping()
{
	if (!Owner) return;

	Owner->StopJumping();
	Owner->CharacterInfo.RemoveState(STATE_JUMP);
}

void UGPMyplayerInputHandler::StartSprinting()
{
	if (!Owner) return;

	Owner->GetCharacterMovement()->MaxWalkSpeed = Owner->NetworkSyncHandler->SprintSpeed;
	Owner->CharacterInfo.AddState(STATE_RUN);
}

void UGPMyplayerInputHandler::StopSprinting()
{
	if (!Owner) return;

	Owner->GetCharacterMovement()->MaxWalkSpeed = Owner->NetworkSyncHandler->WalkSpeed;
	Owner->CharacterInfo.RemoveState(STATE_RUN);
}

void UGPMyplayerInputHandler::AutoAttack()
{
	if (!Owner) return;

	if (Owner->bIsGunnerCharacter() && !Owner->CameraHandler->IsZooming()) return;

	if (!Owner->CombatHandler->IsAutoAttacking() && !Owner->CharacterInfo.HasState(STATE_AUTOATTACK))
	{
		Owner->CharacterInfo.AddState(STATE_AUTOATTACK);
		Owner->NetMgr->SendMyAttackPacket(Owner->GetActorRotation().Yaw);
	}

	Owner->CombatHandler->PlayAutoAttackMontage();
}

void UGPMyplayerInputHandler::ToggleInventory()
{
	if (!Owner) return;
	if (Owner->UIManager)
	{
		Owner->UIManager->ToggleInventory();
	}
}

void UGPMyplayerInputHandler::ResetInventoryToggle()
{
	if (!Owner) return;
	if (Owner->UIManager)
	{
		Owner->UIManager->ResetToggleInventory();
	}
}

void UGPMyplayerInputHandler::OpenInventory()
{
	if (!Owner || !Owner->UIManager) return;
	Owner->UIManager->OpenInventory();
}

void UGPMyplayerInputHandler::CloseInventory()
{
	if (!Owner || !Owner->UIManager) return;
	Owner->UIManager->CloseInventory();
}


void UGPMyplayerInputHandler::OpenSettingWidget()
{
	if (!Owner || !Owner->UIManager) return;
	Owner->UIManager->OpenSettingWidget();
}

void UGPMyplayerInputHandler::TakeItem()
{
	bGetItem = true;

	if (Owner)
	{
		Owner->GetWorldTimerManager().SetTimer(
			GetItemResetTimerHandle,
			[this]()
			{
				bGetItem = false;
			},
			3.0f,  // 딜레이: 3초
			false  // 반복 X
		);
	}
}

void UGPMyplayerInputHandler::StartAiming()
{
	if (!Owner || !Owner->bIsGunnerCharacter()) return;

	Owner->CameraHandler->bWantsToZoom = true;
	Owner->CharacterInfo.AddState(STATE_AIMING);
	Owner->NetMgr->SendMyStartAiming(Owner->GetActorRotation().Yaw);

	if (Owner->UIManager->GunCrosshairWidget)
	{
		Owner->UIManager->GunCrosshairWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGPMyplayerInputHandler::StopAiming()
{
	if (!Owner || !Owner->bIsGunnerCharacter()) return;

	Owner->CameraHandler->bWantsToZoom = false;
	Owner->CharacterInfo.RemoveState(STATE_AIMING);
	Owner->NetMgr->SendMyStopAiming();

	if (Owner->UIManager->GunCrosshairWidget)
	{
		Owner->UIManager->GunCrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGPMyplayerInputHandler::UseSkillQ()
{
	if (!Owner || !Owner->SkillCoolDownHandler || Owner->CombatHandler->IsUsingSkill() || Owner->CharacterInfo.HasState(STATE_SKILL_Q)) return;
	ESkillGroup SkillGroup = Owner->bIsGunnerCharacter() ? ESkillGroup::Throwing : ESkillGroup::HitHard;
	int32 SkillLevel = Owner->CharacterInfo.GetSkillLevel(SkillGroup);
	if (SkillLevel == -1)
		return;

	if (!Owner->SkillCoolDownHandler->CanUseSkill(SkillGroup, SkillLevel))
	{
		return;
	}

	Owner->SkillCoolDownHandler->StartCoolDown(SkillGroup, SkillLevel);

	if (Owner->bIsGunnerCharacter())
	{
		if (!Owner->CameraHandler->IsZooming()) return;

		Owner->CharacterInfo.AddState(STATE_SKILL_Q);
		Owner->CombatHandler->PlayQSkillMontage();
		Owner->NetMgr->SendMyUseSkill(ESkillGroup::Throwing, Owner->GetControlRotation().Yaw);
	}
	else
	{
		Owner->CharacterInfo.AddState(STATE_SKILL_Q);
		Owner->CombatHandler->PlayQSkillMontage();
		Owner->NetMgr->SendMyUseSkill(ESkillGroup::HitHard, Owner->GetControlRotation().Yaw);
	}
}

void UGPMyplayerInputHandler::UseSkillE()
{
	if (!Owner || !Owner->SkillCoolDownHandler || Owner->CombatHandler->IsUsingSkill() || Owner->CharacterInfo.HasState(STATE_SKILL_E)) return;
	ESkillGroup SkillGroup = Owner->bIsGunnerCharacter() ? ESkillGroup::FThrowing : ESkillGroup::Clash;
	int32 SkillLevel = Owner->CharacterInfo.GetSkillLevel(SkillGroup);
	if (SkillLevel == -1)
		return;

	if (!Owner->SkillCoolDownHandler->CanUseSkill(SkillGroup, SkillLevel))
	{
		return;
	}

	Owner->SkillCoolDownHandler->StartCoolDown(SkillGroup, SkillLevel);

	if (Owner->bIsGunnerCharacter())
	{
		if (!Owner->CameraHandler->IsZooming()) return;

		Owner->CharacterInfo.AddState(STATE_SKILL_E);
		Owner->CombatHandler->PlayESkillMontage();
		Owner->NetMgr->SendMyUseSkill(ESkillGroup::FThrowing, Owner->GetControlRotation().Yaw);
	}
	else
	{
		Owner->CharacterInfo.AddState(STATE_SKILL_E);
		Owner->CombatHandler->PlayESkillMontage();
		Owner->NetMgr->SendMyUseSkill(ESkillGroup::Clash, Owner->GetControlRotation().Yaw);
	}
}

void UGPMyplayerInputHandler::UseSkillR()
{
	if (!Owner || !Owner->SkillCoolDownHandler || Owner->CombatHandler->IsUsingSkill() || Owner->CharacterInfo.HasState(STATE_SKILL_R)) return;
	ESkillGroup SkillGroup = Owner->bIsGunnerCharacter() ? ESkillGroup::Anger : ESkillGroup::Whirlwind;
	int32 SkillLevel = Owner->CharacterInfo.GetSkillLevel(SkillGroup);
	if (SkillLevel == -1)
		return;

	if (!Owner->SkillCoolDownHandler->CanUseSkill(SkillGroup, SkillLevel))
	{
		return;
	}

	Owner->SkillCoolDownHandler->StartCoolDown(SkillGroup, SkillLevel);

	if (Owner->bIsGunnerCharacter())
	{
		if (!Owner->CameraHandler->IsZooming()) return;

		Owner->CharacterInfo.AddState(STATE_SKILL_R);
		Owner->CombatHandler->PlayRSkillMontage();
		Owner->NetMgr->SendMyUseSkill(ESkillGroup::Anger, Owner->GetControlRotation().Yaw);
	}
	else
	{
		Owner->CharacterInfo.AddState(STATE_SKILL_R);
		Owner->CombatHandler->PlayRSkillMontage();
		Owner->NetMgr->SendMyUseSkill(ESkillGroup::Whirlwind, Owner->GetControlRotation().Yaw);
	}
}
