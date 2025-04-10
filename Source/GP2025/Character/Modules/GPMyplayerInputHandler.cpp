// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerInputHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/GPPlayerController.h"
#include "Inventory/GPInventory.h"
#include "Blueprint/UserWidget.h"
#include "Network/GPNetworkManager.h"
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
	LoadAction(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Interaction.IA_Interaction'"), InteractionAction);
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

	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::Jump);
	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::StopJumping);

	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::Move);
	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::Look);

	EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::StartSprinting);
	EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::StopSprinting);

	EnhancedInput->BindAction(AutoAttackAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::AutoAttack);

	EnhancedInput->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::ToggleInventory);
	EnhancedInput->BindAction(InventoryAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::ResetInventoryToggle);

	EnhancedInput->BindAction(SettingAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::OpenSettingWidget);
	//EnhancedInput->BindAction(Owner->InteractionAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::ProcessInteraction);

	EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::StartAiming);
	EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Completed, this, &UGPMyplayerInputHandler::StopAiming);

	EnhancedInput->BindAction(SkillQAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::UseSkillQ);
	EnhancedInput->BindAction(SkillEAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::UseSkillE);
	EnhancedInput->BindAction(SkillRAction, ETriggerEvent::Triggered, this, &UGPMyplayerInputHandler::UseSkillR);
}


void UGPMyplayerInputHandler::Move(const FInputActionValue& Value)
{
	if (!Owner) return;

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
	if (!Owner) return;

	Owner->Jump();
	Owner->isJumpStart = true;
	Owner->CharacterInfo.RemoveState(STATE_IDLE);
	Owner->CharacterInfo.AddState(STATE_JUMP);
	Owner->SetupMasterPose();
}

void UGPMyplayerInputHandler::StopJumping()
{
	if (!Owner) return;

	Owner->StopJumping();
	Owner->CharacterInfo.RemoveState(STATE_JUMP);
	Owner->SetupMasterPose();
}

void UGPMyplayerInputHandler::StartSprinting()
{
	if (!Owner) return;

	Owner->GetCharacterMovement()->MaxWalkSpeed = Owner->SprintSpeed;
	Owner->CharacterInfo.AddState(STATE_RUN);
	Owner->SetupMasterPose();
}

void UGPMyplayerInputHandler::StopSprinting()
{
	if (!Owner) return;

	Owner->GetCharacterMovement()->MaxWalkSpeed = Owner->WalkSpeed;
	Owner->CharacterInfo.RemoveState(STATE_RUN);
	Owner->SetupMasterPose();
}

void UGPMyplayerInputHandler::AutoAttack()
{
	if (!Owner) return;
	if (Owner->bIsGunnerCharacter() && !Owner->IsZooming()) return;

	if (!Owner->bIsAutoAttacking && !Owner->CharacterInfo.HasState(STATE_AUTOATTACK))
	{
		Owner->CharacterInfo.AddState(STATE_AUTOATTACK);
		Owner->NetMgr->SendPlayerAttackPacket(Owner->GetActorRotation().Yaw);
	}

	Owner->ProcessAutoAttackCommand();
	Owner->SetupMasterPose();
}

void UGPMyplayerInputHandler::ToggleInventory()
{
	if (!Owner || bInventoryToggled) return;

	bInventoryToggled = true;

	if (Owner->InventoryWidget)
	{
		if (Owner->InventoryWidget->IsInViewport())
			CloseInventory();
		else
			OpenInventory();
	}
}

void UGPMyplayerInputHandler::ResetInventoryToggle()
{
	if (!Owner) return;
	bInventoryToggled = false;
}

void UGPMyplayerInputHandler::OpenInventory()
{
	if (!Owner || !Owner->InventoryWidget) return;

	if (!Owner->InventoryWidget->IsInViewport())
	{
		Owner->InventoryWidget->AddToViewport();

		APlayerController* PC = Cast<AGPPlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}

		UGPInventory* CastInventory = Cast<UGPInventory>(Owner->InventoryWidget);
		if (CastInventory)
		{
			CastInventory->SetGold(Owner->CharacterInfo.Gold);
		}
	}

	if (Owner->InGameWidget && !Owner->InGameWidget->IsInViewport())
	{
		Owner->InGameWidget->AddToViewport();
	}
}

void UGPMyplayerInputHandler::CloseInventory()
{
	if (!Owner || !Owner->InventoryWidget) return;

	if (Owner->InventoryWidget->IsInViewport())
	{
		Owner->InventoryWidget->RemoveFromParent();

		APlayerController* PC = Cast<AGPPlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}


void UGPMyplayerInputHandler::OpenSettingWidget()
{
	if (!Owner || !Owner->SettingWidget || Owner->SettingWidget->IsInViewport()) return;

	Owner->SettingWidget->AddToViewport();

	APlayerController* PC = Cast<APlayerController>(Owner->GetController());
	if (PC)
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeGameAndUI());
	}
}

//void UGPMyplayerInputHandler::ProcessInteraction()
//{
//	if (!Owner) return;
//
//	Owner->bInteractItem = true;
//
//	FTimerDelegate Delegate;
//	Delegate.BindUFunction(this, FName("ResetInteractItem"));
//	Owner->GetWorldTimerManager().SetTimer(
//		Owner->InteractItemTimerHandle,
//		Delegate,
//		2.0f,
//		false
//	);
//}
//
//void UGPMyplayerInputHandler::ResetInteractItem()
//{
//
//}

void UGPMyplayerInputHandler::StartAiming()
{
	if (!Owner || !Owner->bIsGunnerCharacter()) return;

	Owner->bWantsToZoom = true;

	if (Owner->GunCrosshairWidget)
	{
		Owner->GunCrosshairWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGPMyplayerInputHandler::StopAiming()
{
	if (!Owner || !Owner->bIsGunnerCharacter()) return;

	Owner->bWantsToZoom = false;

	if (Owner->GunCrosshairWidget)
	{
		Owner->GunCrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGPMyplayerInputHandler::UseSkillQ()
{
	if (!Owner || Owner->bIsUsingSkill || Owner->CharacterInfo.HasState(STATE_SKILL_Q)) return;

	if (Owner->bIsGunnerCharacter())
	{
		if (!Owner->IsZooming()) return;

		Owner->CharacterInfo.AddState(STATE_SKILL_Q);
		Owner->ProcessThrowingCommand();
		Owner->NetMgr->SendPlayerUseSkill(ESkillGroup::Throwing);
	}
	else
	{
		Owner->CharacterInfo.AddState(STATE_SKILL_Q);
		Owner->ProcessHitHardCommand();
		Owner->NetMgr->SendPlayerUseSkill(ESkillGroup::HitHard);
	}

	Owner->SetupMasterPose();
}

void UGPMyplayerInputHandler::UseSkillE()
{
	if (!Owner || Owner->bIsUsingSkill || Owner->CharacterInfo.HasState(STATE_SKILL_E)) return;

	if (Owner->bIsGunnerCharacter())
	{
		if (!Owner->IsZooming()) return;

		Owner->CharacterInfo.AddState(STATE_SKILL_E);
		Owner->ProcessFThrowingCommand();
		Owner->NetMgr->SendPlayerUseSkill(ESkillGroup::FThrowing);
	}
	else
	{
		Owner->CharacterInfo.AddState(STATE_SKILL_E);
		Owner->ProcessClashCommand();
		Owner->NetMgr->SendPlayerUseSkill(ESkillGroup::Clash);
	}

	Owner->SetupMasterPose();
}

void UGPMyplayerInputHandler::UseSkillR()
{
	if (!Owner || Owner->bIsUsingSkill || Owner->CharacterInfo.HasState(STATE_SKILL_R)) return;

	if (Owner->bIsGunnerCharacter())
	{
		if (!Owner->IsZooming()) return;

		Owner->CharacterInfo.AddState(STATE_SKILL_R);
		Owner->ProcessAngerCommand();
		Owner->NetMgr->SendPlayerUseSkill(ESkillGroup::Anger);
	}
	else
	{
		Owner->CharacterInfo.AddState(STATE_SKILL_R);
		Owner->ProcessWhirlwindCommand();
		Owner->NetMgr->SendPlayerUseSkill(ESkillGroup::Whirlwind);
	}

	Owner->SetupMasterPose();
}