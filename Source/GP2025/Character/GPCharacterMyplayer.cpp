// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GPCharacterMyplayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GPCharacterControlData.h"
#include "GPCharacterMonster.h"
#include "InputMappingContext.h"
#include "Physics/GPCollision.h"
#include "Network/GPNetworkManager.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/GPInventory.h"
#include "Player/GPPlayerController.h"
#include "Weapons/GPWeaponBase.h"
#include "UI/GPInGameWidget.h"

AGPCharacterMyplayer::AGPCharacterMyplayer()
{
	// 카메라 세팅
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Input 세팅
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/PlayerInput/IMC_PlayerIMC.IMC_PlayerIMC'"));
	if (nullptr != InputMappingContextRef.Object)
	{
		DefaultMappingContext = InputMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Move.IA_Move'"));
	if (nullptr != InputActionMoveRef.Object)
	{
		MoveAction = InputActionMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Look.IA_Look'"));
	if (nullptr != InputActionLookRef.Object)
	{
		LookAction = InputActionLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSprintRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Sprint.IA_Sprint'"));
	if (InputActionSprintRef.Object)
	{
		SprintAction = InputActionSprintRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAutoAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_AutoAttack.IA_AutoAttack'"));
	if (InputActionAutoAttackRef.Object)
	{
		AutoAttackAction = InputActionAutoAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionInventoryRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Inventory.IA_Inventory'"));
	if (InputActionInventoryRef.Object)
	{
		InventoryAction = InputActionInventoryRef.Object;
	}



	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSettingRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_ESC.IA_ESC'"));
	if (InputActionSettingRef.Object)
	{
		SettingAction = InputActionSettingRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InteractionActionSettingRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_Interaction.IA_Interaction'"));
	if (InteractionActionSettingRef.Object)
	{
		InteractionAction = InteractionActionSettingRef.Object;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/Inventory/Widgets/WBP_Inventory"));

	if (WidgetBPClass.Succeeded())
	{
		InventoryWidgetClass = WidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> SettingWidgetBPClass(TEXT("/Game/UI/WBP_PauseScreen"));

	if (SettingWidgetBPClass.Succeeded())
	{
		SettingWidgetClass = SettingWidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameWidgetBPClass(TEXT("/Game/UI/WBP_InGame"));
	if (InGameWidgetBPClass.Succeeded())
	{
		InGameWidgetClass = InGameWidgetBPClass.Class;
	}

	// 기본 캐릭터 타입
	CurrentCharacterType = (uint8)Type::EPlayer::WARRIOR;
}

void AGPCharacterMyplayer::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterType(CurrentCharacterType);

	auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (NetworkMgr)
		NetworkMgr->SetMyPlayer(Cast<AGPCharacterPlayer>(this));

	LastLocation = GetActorLocation();
	LastRotationYaw = GetActorRotation().Yaw;
	LastSendPlayerInfo = CharacterInfo;

	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);
	}

	if (SettingWidgetClass)
	{
		SettingWidget = CreateWidget<UUserWidget>(GetWorld(), SettingWidgetClass);
	}

	if (InGameWidgetClass)
	{
		InGameWidget = CreateWidget<UUserWidget>(GetWorld(), InGameWidgetClass);
		if (InGameWidget)
		{
			InGameWidget->AddToViewport();
			APlayerController* PC = Cast<AGPPlayerController>(GetController());
			if (PC)
			{
				PC->SetShowMouseCursor(false);
				PC->SetInputMode(FInputModeGameOnly());
			}
			UE_LOG(LogTemp, Warning, TEXT("InGameWidget successfully added to viewport."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create InGameWidget."));
		}
	}
}

void AGPCharacterMyplayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (!NetworkMgr) return;

	MovePacketSendTimer -= DeltaTime;
	FVector CurrentLocation = GetActorLocation();
	float CurrentRotationYaw = GetActorRotation().Yaw;

	CharacterInfo.SetLocation(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z);
	CharacterInfo.Yaw = CurrentRotationYaw;
	CharacterInfo.Stats.Speed = GetVelocity().Size();

	float DistanceMoved = FVector::DistSquared(CurrentLocation, LastLocation);
	LastLocation = CurrentLocation;

	const float YawThreshold = 10.f;
	float YawDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotationYaw, LastRotationYaw));
	bool bYawChanged = (YawDifference > YawThreshold);
	LastRotationYaw = CurrentRotationYaw;

	// IDLE 상태인지 아닌지 판단 - IDLE 조건 1 : 이동 거리가 0.5cm 이하
	const float NotMovedThreshold = 2.f;
	if ((DistanceMoved >= NotMovedThreshold))
	{
		CharacterInfo.RemoveState(STATE_IDLE);
	}
	else if ((DistanceMoved < NotMovedThreshold))
	{
		CharacterInfo.AddState(STATE_IDLE);
	}

	// Jump() 시 패킷 전송
	if (isJumpStart && !bWasJumping)
	{
		isJumpStart = false;
		bWasJumping = true;
		NetworkMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = CharacterInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Jump Issue"));
		return;
	}

	// 착지 시 bWasJumping 초기화
	if (GetCharacterMovement()->IsMovingOnGround())
		bWasJumping = false;

	const float AirThreshold = 10.f;
	// 점프 후 착지를 안하고 플레이어가 계속 공중에 떠 있다면 떨어뜨리기 위해 패킷 전송
	if (CharacterInfo.HasState(STATE_IDLE) && !CharacterInfo.HasState(STATE_JUMP)
		&& (LastSendPlayerInfo.Pos.Z - GroundZLocation) > AirThreshold)
	{
		CharacterInfo.Pos.Z = GroundZLocation;
		CharacterInfo.Stats.Speed = LastSendPlayerInfo.HasState(STATE_RUN) ? SprintSpeed : WalkSpeed;

		NetworkMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = CharacterInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Air Fixed Issue"));
		return;
	}

	// IDLE 상태에서 캐릭터의 회전이 변경되었을 때 패킷 전송
	if (bYawChanged && CharacterInfo.HasState(STATE_IDLE))
	{
		NetworkMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = CharacterInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Rotation Issue"));
		return;
	}

	// 일정 시간마다 서버에 패킷 전송
	if (MovePacketSendTimer <= 0 || (CharacterInfo.HasState(STATE_IDLE) && DistanceMoved >= NotMovedThreshold))
	{
		MovePacketSendTimer = PACKETSENDTIME;

		// IDLE 상태가 아니거나 일정 거리 이상 이동한 경우 패킷 전송
		if (!CharacterInfo.HasState(STATE_IDLE) || DistanceMoved >= NotMovedThreshold)
		{
			NetworkMgr->SendPlayerMovePacket();
			LastSendPlayerInfo = CharacterInfo;

			if (MovePacketSendTimer <= 0)
			{
				UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : periodically"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : I am Idle but when I moved"));
			}
		}
	}
}

void AGPCharacterMyplayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGPCharacterMyplayer::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGPCharacterMyplayer::StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGPCharacterMyplayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGPCharacterMyplayer::Look);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AGPCharacterMyplayer::StartSprinting);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGPCharacterMyplayer::StopSprinting);
	EnhancedInputComponent->BindAction(AutoAttackAction, ETriggerEvent::Triggered, this, &AGPCharacterMyplayer::AutoAttack);
	EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &AGPCharacterMyplayer::ToggleInventory);
	EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Completed, this, &AGPCharacterMyplayer::ResetInventoryToggle);
	EnhancedInputComponent->BindAction(SettingAction, ETriggerEvent::Triggered, this, &AGPCharacterMyplayer::OpenSettingWidget);
	EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &AGPCharacterMyplayer::ProcessInteraction);

}

void AGPCharacterMyplayer::SetCharacterType(ECharacterType NewCharacterType)
{
	Super::SetCharacterType(NewCharacterType);

	UGPCharacterControlData* NewCharacterData = CharacterTypeManager[NewCharacterType];
	check(NewCharacterData);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterData->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterType = NewCharacterType;
}

void AGPCharacterMyplayer::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
	Super::SetCharacterData(CharacterControlData);

	WalkSpeed = CharacterControlData->WalkSpeed;
	SprintSpeed = CharacterControlData->SprintSpeed;
}


void AGPCharacterMyplayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void AGPCharacterMyplayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGPCharacterMyplayer::Jump()
{
	Super::Jump();
	isJumpStart = true;
	CharacterInfo.RemoveState(STATE_IDLE);
	CharacterInfo.AddState(STATE_JUMP);
	SetupMasterPose();

}

void AGPCharacterMyplayer::StopJumping()
{
	Super::StopJumping();
	CharacterInfo.RemoveState(STATE_JUMP);
	SetupMasterPose();
}

void AGPCharacterMyplayer::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	CharacterInfo.AddState(STATE_RUN);
	SetupMasterPose();
}

void AGPCharacterMyplayer::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	CharacterInfo.RemoveState(STATE_RUN);
	SetupMasterPose();
}

void AGPCharacterMyplayer::AutoAttack()
{
	if (bIsAutoAttacking == false && !CharacterInfo.HasState(STATE_AUTOATTACK))
	{
		CharacterInfo.AddState(STATE_AUTOATTACK);
		auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
		NetworkMgr->SendPlayerAttackPacket();
	}

	ProcessAutoAttackCommand();
	SetupMasterPose();
}

void AGPCharacterMyplayer::ToggleInventory()
{
	if (bInventoryToggled) return; 

	bInventoryToggled = true;

	if (InventoryWidget)
	{
		if (InventoryWidget->IsInViewport())
		{
			CloseInventory();
		}
		else
		{
			OpenInventory();
		}
	}
}

void AGPCharacterMyplayer::OpenInventory()
{
	if (InventoryWidget && !InventoryWidget->IsInViewport())
	{
		InventoryWidget->AddToViewport();

		APlayerController* PC = Cast<AGPPlayerController>(GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI()); 
		}
	}

	// InGameWidget 유지
	if (InGameWidget && !InGameWidget->IsInViewport())
	{
		InGameWidget->AddToViewport();
	}
}

void AGPCharacterMyplayer::CloseInventory()
{
	if (InventoryWidget && InventoryWidget->IsInViewport())
	{
		InventoryWidget->RemoveFromParent();

		APlayerController* PC = Cast<AGPPlayerController>(GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly()); 
		}
	}
}

void AGPCharacterMyplayer::ResetInventoryToggle()
{
	bInventoryToggled = false; 
}

void AGPCharacterMyplayer::OpenSettingWidget()
{
	if (SettingWidget && !SettingWidget->IsInViewport())
	{
		SettingWidget->AddToViewport();

		APlayerController* PC = Cast<AGPPlayerController>(GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}

void AGPCharacterMyplayer::ProcessInteraction()
{
	bInteractItem = true;
	GetWorldTimerManager().SetTimer(
		InteractItemTimerHandle,             
		this,                              
		&AGPCharacterMyplayer::ResetInteractItem, 
		2.0f,                              
		false);
}

void AGPCharacterMyplayer::ResetInteractItem()
{
	bInteractItem = false;
}

UGPInventory* AGPCharacterMyplayer::GetInventoryWidget()
{
	UGPInventory* CastInventory = Cast<UGPInventory>(InventoryWidget);
	if (!CastInventory)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast InventoryWidget to UGPInventory"));
		return nullptr;
	}

	return CastInventory;
}

void AGPCharacterMyplayer::AttackHitCheck()
{
	if (WeaponActor)
	{
		WeaponActor->AttackHitCheck();
	}
}

void AGPCharacterMyplayer::SetCharacterInfo(FInfoData& CharacterInfo_)
{
	Super::SetCharacterInfo(CharacterInfo_);

	UGPInGameWidget* InGame = Cast<UGPInGameWidget>(InGameWidget);

	if (!InGame) return;

	InGame->UpdateHealthBar(CharacterInfo_.Stats.Hp / CharacterInfo_.Stats.MaxHp);
	InGame->UpdateExpBar(CharacterInfo_.Stats.Exp / CharacterInfo_.Stats.MaxExp);
	InGame->UpdatePlayerLevel(CharacterInfo_.Stats.Level);
}


