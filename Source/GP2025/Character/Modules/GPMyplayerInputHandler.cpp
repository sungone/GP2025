// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerInputHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/Modules/GPMyplayerNetworkSyncHandler.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Character/Modules/GPMyplayerSoundManager.h"
#include "Character/Modules/GPCharacterCombatHandler.h"
#include "Character/GPCharacterNPC.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Skill/GPSkillCoolDownHandler.h"
#include "Player/GPPlayerController.h"
#include "UI/GPQuestWidget.h"
#include "Inventory/GPInventory.h"
#include "UI/GPInGameWidget.h"
#include "Blueprint/UserWidget.h"
#include "Network/GPNetworkManager.h"
#include "Network/GPGameInstance.h"
#include "TimerManager.h"


UGPMyplayerInputHandler::UGPMyplayerInputHandler()
{
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
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_TakeItem.IA_TakeItem'"), TakeInteractionAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Zoom.IA_Zoom'"), ZoomAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_SkillQ.IA_SkillQ'"), SkillQAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_SkillE.IA_SkillE'"), SkillEAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_SkillR.IA_SkillR'"), SkillRAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Accept.IA_Accept'"), AcceptAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Refuse.IA_Refuse'"), RefuseAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Interaction.IA_Interaction'"), InteractionAction);
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_EnterKey.IA_EnterKey'"), EnterKeyAction); 
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

	EnhancedInput->BindAction(AutoAttackAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::AutoAttack);

	EnhancedInput->BindAction(InventoryAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::ToggleInventory);
	EnhancedInput->BindAction(InventoryAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::ResetInventoryToggle);

	EnhancedInput->BindAction(SettingAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::ToggleSettingWidget);
	EnhancedInput->BindAction(TakeInteractionAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::TakeInteraction);

	EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::StartAiming);
	EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::StopAiming);

	EnhancedInput->BindAction(SkillQAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::UseSkillQ);
	EnhancedInput->BindAction(SkillEAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::UseSkillE);
	EnhancedInput->BindAction(SkillRAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::UseSkillR);

	EnhancedInput->BindAction(AcceptAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::Accept);
	EnhancedInput->BindAction(RefuseAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::Refuse);
	EnhancedInput->BindAction(InteractionAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::Interact);
	EnhancedInput->BindAction(EnterKeyAction, ETriggerEvent::Started, this, &UGPMyplayerInputHandler::EnterChatting);
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

	if (Owner->SoundManager && Owner->SoundManager->PlayerJumpSound)
	{
		Owner->SoundManager->PlaySFX(Owner->SoundManager->PlayerJumpSound);
	}

	Owner->NetworkSyncHandler->isJumpStart = true;
	Owner->CharacterInfo.RemoveState(STATE_IDLE);
	Owner->CharacterInfo.AddState(STATE_JUMP);

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
	Owner->CharacterInfo.RemoveState(STATE_WALK);
	Owner->CharacterInfo.AddState(STATE_RUN);
}

void UGPMyplayerInputHandler::StopSprinting()
{
	if (!Owner) return;

	Owner->GetCharacterMovement()->MaxWalkSpeed = Owner->NetworkSyncHandler->WalkSpeed;
	Owner->CharacterInfo.RemoveState(STATE_RUN);
	Owner->CharacterInfo.AddState(STATE_WALK);
}

void UGPMyplayerInputHandler::AutoAttack()
{
	if (!Owner)
	{
		return;
	}

	bool bIsAlreadyAttacking = Owner->CombatHandler->IsAutoAttacking();
	bool bIsUsingSkill = Owner->CombatHandler->IsUsingSkill();
	bool bHasWeapon = Owner->HasWeaponEquipped();
	// bool bHasAttackState = Owner->CharacterInfo.HasState(STATE_AUTOATTACK);

	if (bHasWeapon)
	{
		if (Owner->bIsGunnerCharacter() && !Owner->CameraHandler->IsZooming())
		{
			return;
		}

		if (!bIsAlreadyAttacking && !bIsUsingSkill)
		{
			Owner->CharacterInfo.AddState(STATE_AUTOATTACK);
			Owner->CombatHandler->PlayAutoAttackMontage();
			FTimerHandle TimerHandle;
			Owner->GetWorldTimerManager().SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([this]()
					{
						if (Owner)
						{
							Owner->NetMgr->SendMyAttackPacket(Owner->GetActorRotation().Yaw, Owner->GetActorLocation());
						}
					}),
				0.2f,
				false
			);
		}

		if (Owner->bIsGunnerCharacter())
		{
			Owner->GunnerAttackCameraShake();
		}
	}
	else
	{
		if (!bIsAlreadyAttacking && !bIsUsingSkill)
		{
			Owner->CharacterInfo.AddState(STATE_AUTOATTACK);
			Owner->CombatHandler->PlayAutoAttackMontage();
			FTimerHandle TimerHandle;
			Owner->GetWorldTimerManager().SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([this]()
					{
						if (Owner)
						{
							Owner->NetMgr->SendMyAttackPacket(Owner->GetActorRotation().Yaw, Owner->GetActorLocation());
						}
					}),
				0.1f,  
				false
			);
		}
	}
}

void UGPMyplayerInputHandler::ToggleInventory()
{
	if (!Owner) return;

	if (Owner->UIManager)
	{
		Owner->UIManager->ToggleInventory();
	}

	if (Owner->SoundManager)
	{
		Owner->SoundManager->PlaySFX(Owner->SoundManager->ClickSound);
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


void UGPMyplayerInputHandler::ToggleSettingWidget()
{
	if (!Owner || !Owner->UIManager) return;
	Owner->UIManager->ToggleSettingWidget();
}

void UGPMyplayerInputHandler::Accept()
{
	if (!Owner || !Owner->UIManager) return;

	if (UGPQuestWidget* QuestWidget = Owner->UIManager->CurrentQuestWidget)
	{
		QuestWidget->OnQuestAccepted();
	}

	if (Owner->SoundManager)
	{
		Owner->SoundManager->PlaySFX(Owner->SoundManager->ClickSound);
	}
}

void UGPMyplayerInputHandler::Refuse()
{
	if (!Owner || !Owner->UIManager) return;

	if (UGPQuestWidget* QuestWidget = Owner->UIManager->CurrentQuestWidget)
	{
		QuestWidget->OnQuestExit();
	}

	if (Owner->SoundManager)
	{
		Owner->SoundManager->PlaySFX(Owner->SoundManager->ClickSound);
	}
}

void UGPMyplayerInputHandler::Interact()
{
	if (!Owner || !Owner->UIManager) return;

	if (!bIsFriendBoxOpen)
	{
		Owner->UIManager->OpenFriendBox();
		bIsFriendBoxOpen = true;
	}
	else
	{
		Owner->UIManager->CloseFriendBox();
		bIsFriendBoxOpen = false;
	}

	if (Owner->SoundManager && Owner->SoundManager->ClickSound)
	{
		Owner->SoundManager->PlaySFX(Owner->SoundManager->ClickSound);
	}
}

void UGPMyplayerInputHandler::EnterChatting()
{
	if (!Owner || !Owner->UIManager) return;
	Owner->UIManager->FocusChatInput();
}

void UGPMyplayerInputHandler::TakeInteraction()
{
	if (!Owner) return;

	// Item Drop
	bGetTakeItem = true;
	Owner->GetWorldTimerManager().SetTimer(
		GetInteractionResetTimerHandle,
		[this]() { bGetTakeItem = false; },
		1.f,
		false
	);

	// NPC
	if (AGPCharacterNPC* NPC = Cast<AGPCharacterNPC>(CurrentInteractionTarget))
	{
		NPC->CheckAndHandleInteraction(Cast<AGPCharacterMyplayer>(Owner));

		if (Owner->SoundManager)
		{
			Owner->SoundManager->PlaySFX(Owner->SoundManager->ClickSound);
		}
	}
}

void UGPMyplayerInputHandler::StartAiming()
{
	if (!Owner || !Owner->bIsGunnerCharacter()) return;

	if (!Owner->HasWeaponEquipped())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Aiming] Gunner has no weapon equipped. Zoom disabled."));
		return;
	}

	if (Owner->SoundManager)
	{
		Owner->SoundManager->PlaySFX(Owner->SoundManager->GunnerPlayerZoomSound);
	}

	Owner->CameraHandler->bWantsToZoom = true;
	Owner->CharacterInfo.AddState(STATE_AIMING);
	Owner->NetMgr->SendMyStartAiming(Owner->GetActorRotation().Yaw, Owner->GetActorLocation());

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
	bool bIsAlreadyAttacking = Owner->CombatHandler->IsAutoAttacking();
	if (!Owner || !Owner->SkillCoolDownHandler || Owner->CombatHandler->IsUsingSkill() 
		 || bIsAlreadyAttacking || !Owner->HasWeaponEquipped()) return;

	ESkillGroup SkillGroup = Owner->bIsGunnerCharacter() ? ESkillGroup::Throwing : ESkillGroup::HitHard;
	int32 SkillLevel = Owner->CharacterInfo.GetSkillLevel(SkillGroup);
	if (SkillLevel == -1)
		return;

	if (!Owner->SkillCoolDownHandler->CanUseSkill(SkillGroup, SkillLevel))
	{
		return;
	}

	if (Owner->bIsGunnerCharacter())
	{
		if (!Owner->CameraHandler->IsZooming()) return;

		Owner->SkillCoolDownHandler->StartCoolDown(SkillGroup, SkillLevel);
		Owner->CharacterInfo.AddState(STATE_SKILL_Q);
		Owner->CombatHandler->PlayQSkillMontage();
		Owner->NetMgr->SendMyUseSkillStart(ESkillGroup::Throwing, Owner->GetControlRotation().Yaw, Owner->GetActorLocation());
		Owner->GunnerAttackCameraShake();
		FTimerHandle TimerHandle;
		Owner->GetWorldTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this]()
				{
					if (Owner)
					{
						Owner->NetMgr->SendMyAttackPacket(Owner->GetActorRotation().Yaw, Owner->GetActorLocation());
					}
				}),
			0.2f,  // ���� �ð� (��)
			false
		);
	}
	else
	{
		Owner->SkillCoolDownHandler->StartCoolDown(SkillGroup, SkillLevel);
		Owner->CharacterInfo.AddState(STATE_SKILL_Q);
		Owner->CombatHandler->PlayQSkillMontage();
		Owner->NetMgr->SendMyUseSkillStart(ESkillGroup::HitHard, Owner->GetControlRotation().Yaw, Owner->GetActorLocation());
		FTimerHandle TimerHandle;
		Owner->GetWorldTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this]()
				{
					if (Owner)
					{
						Owner->NetMgr->SendMyAttackPacket(Owner->GetActorRotation().Yaw, Owner->GetActorLocation());
					}
				}),
			0.1f,  // ���� �ð� (��)
			false
		);
	}
}

void UGPMyplayerInputHandler::UseSkillE()
{
	if (!Owner || !Owner->SkillCoolDownHandler || Owner->CombatHandler->IsUsingSkill() || !Owner->HasWeaponEquipped()) return;

	ESkillGroup SkillGroup = Owner->bIsGunnerCharacter() ? ESkillGroup::FThrowing : ESkillGroup::Clash;
	int32 SkillLevel = Owner->CharacterInfo.GetSkillLevel(SkillGroup);
	if (SkillLevel == -1)
		return;

	if (!Owner->SkillCoolDownHandler->CanUseSkill(SkillGroup, SkillLevel))
	{
		return;
	}

	if (Owner->bIsGunnerCharacter())
	{
		if (!Owner->CameraHandler->IsZooming()) return;

		Owner->SkillCoolDownHandler->StartCoolDown(SkillGroup, SkillLevel);
		Owner->CharacterInfo.AddState(STATE_SKILL_E);
		Owner->CombatHandler->PlayESkillMontage();
		Owner->NetMgr->SendMyUseSkillStart(ESkillGroup::FThrowing, Owner->GetControlRotation().Yaw, Owner->GetActorLocation());
		Owner->GunnerAttackCameraShake();
		FTimerHandle TimerHandle;
		Owner->GetWorldTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this]()
				{
					if (Owner)
					{
						Owner->NetMgr->SendMyAttackPacket(Owner->GetActorRotation().Yaw, Owner->GetActorLocation());
					}
				}),
			0.2f,
			false
		);
	}
	else
	{
		Owner->SkillCoolDownHandler->StartCoolDown(SkillGroup, SkillLevel);
		Owner->CharacterInfo.AddState(STATE_SKILL_E);
		Owner->NetMgr->SendMyUseSkillStart(ESkillGroup::Clash, Owner->GetControlRotation().Yaw, Owner->GetActorLocation());
		Owner->CombatHandler->StartDash(); // ���⼭ ���� ó�� ��

		float BoostPlayRate = 4.0f; // ���ݼӵ� ��Ÿ�� ���� �ð�
		float BoostDuration = 10.f; // ���ݼӵ� ���� ���� �ð�
		Owner->CombatHandler->ApplyAttackSpeedBoost(BoostPlayRate, BoostDuration);
	}
}

void UGPMyplayerInputHandler::UseSkillR()
{
	bool bIsAlreadyAttacking = Owner->CombatHandler->IsAutoAttacking();
	if (!Owner || !Owner->SkillCoolDownHandler || Owner->CombatHandler->IsUsingSkill() 
		 || bIsAlreadyAttacking || !Owner->HasWeaponEquipped()) return;

	ESkillGroup SkillGroup = Owner->bIsGunnerCharacter() ? ESkillGroup::Anger : ESkillGroup::Whirlwind;
	int32 SkillLevel = Owner->CharacterInfo.GetSkillLevel(SkillGroup);
	if (SkillLevel == -1)
		return;

	if (!Owner->SkillCoolDownHandler->CanUseSkill(SkillGroup, SkillLevel))
	{
		return;
	}

	if (Owner->bIsGunnerCharacter())
	{
		 // if (!Owner->CameraHandler->IsZooming()) return;

		float BoostPlayRate = 4.0f; // ���ݼӵ� ��Ÿ�� ���� �ð�
		float BoostDuration = 10.f; // ���ݼӵ� ���� ���� �ð�
		Owner->CombatHandler->ApplyAttackSpeedBoost(BoostPlayRate, BoostDuration);

		Owner->SkillCoolDownHandler->StartCoolDown(SkillGroup, SkillLevel);
		Owner->CharacterInfo.AddState(STATE_SKILL_R);

		Owner->CombatHandler->PlayRSkillMontage();
		Owner->NetMgr->SendMyUseSkillStart(ESkillGroup::Anger, Owner->GetControlRotation().Yaw, Owner->GetActorLocation());
	}
	else
	{
		int32 HitCount = 3;      // ���� Ƚ��
		float Interval = 0.4f;   // ���� ����

		Owner->SkillCoolDownHandler->StartCoolDown(SkillGroup, SkillLevel);
		Owner->CharacterInfo.AddState(STATE_SKILL_R);
		Owner->CombatHandler->PlayRSkillMontage(); 
		// Owner->NetMgr->SendMyUseSkill(ESkillGroup::Whirlwind, Owner->GetControlRotation().Yaw, Owner->GetActorLocation());
		Owner->NetMgr->SendMyUseSkillStart(ESkillGroup::Whirlwind, Owner->GetControlRotation().Yaw, 
			Owner->GetActorLocation());
		Owner->CombatHandler->PlayMultiHitSkill(HitCount, Interval); // ���⼭ ���� ó�� ��
	}
}
