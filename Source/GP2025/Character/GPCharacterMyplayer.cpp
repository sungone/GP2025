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

AGPCharacterMyplayer::AGPCharacterMyplayer()
{
	// ī�޶� ����
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Input ����
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

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/Inventory/Widgets/WBP_Inventory"));

	if (WidgetBPClass.Succeeded())
	{
		InventoryWidgetClass = WidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> SettingWidgetBPClass(TEXT("/Game/Inventory/Widgets/WBP_PauseScreen"));

	if (SettingWidgetBPClass.Succeeded())
	{
		SettingWidgetClass = SettingWidgetBPClass.Class;
	}

	// �⺻ ĳ���� Ÿ���� ���� ĳ���ͷ�
	CurrentCharacterType = (uint8)Type::EPlayer::WARRIOR;
}

void AGPCharacterMyplayer::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->SetWorldScale3D(FVector(1.0f));
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
	CharacterInfo.Speed = GetVelocity().Size();

	float DistanceMoved = FVector::DistSquared(CurrentLocation, LastLocation);
	LastLocation = CurrentLocation;

	const float YawThreshold = 10.f;
	float YawDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotationYaw, LastRotationYaw));
	bool bYawChanged = (YawDifference > YawThreshold);
	LastRotationYaw = CurrentRotationYaw;

	// IDLE �������� �ƴ��� �Ǵ� - IDLE ���� 1 : �̵� �Ÿ��� 0.5cm ����
	const float NotMovedThreshold = 2.f;
	if ((DistanceMoved >= NotMovedThreshold))
	{
		CharacterInfo.RemoveState(STATE_IDLE);
	}
	else if ((DistanceMoved < NotMovedThreshold))
	{
		CharacterInfo.AddState(STATE_IDLE);
	}

	// Jump() �� ��Ŷ ����
	if (isJumpStart && !bWasJumping)
	{
		isJumpStart = false;
		bWasJumping = true;
		NetworkMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = CharacterInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Jump Issue"));
		return;
	}

	// ���� �� bWasJumping �ʱ�ȭ
	if (GetCharacterMovement()->IsMovingOnGround())
		bWasJumping = false;

	const float AirThreshold = 10.f;
	// ���� �� ������ ���ϰ� �÷��̾ ��� ���߿� �� �ִٸ� ����߸��� ���� ��Ŷ ����
	if (CharacterInfo.HasState(STATE_IDLE) && !CharacterInfo.HasState(STATE_JUMP)
		&& (LastSendPlayerInfo.Pos.Z - GroundZLocation) > AirThreshold)
	{
		CharacterInfo.Pos.Z = GroundZLocation;
		CharacterInfo.Speed = LastSendPlayerInfo.HasState(STATE_RUN) ? SprintSpeed : WalkSpeed;

		NetworkMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = CharacterInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Air Fixed Issue"));
		return;
	}

	// IDLE ���¿��� ĳ������ ȸ���� ����Ǿ��� �� ��Ŷ ����
	if (bYawChanged && CharacterInfo.HasState(STATE_IDLE))
	{
		NetworkMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = CharacterInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Rotation Issue"));
		return;
	}

	// ���� �ð����� ������ ��Ŷ ����
	if (MovePacketSendTimer <= 0 || (CharacterInfo.HasState(STATE_IDLE) && DistanceMoved >= NotMovedThreshold))
	{
		MovePacketSendTimer = PACKETSENDTIME;

		// IDLE ���°� �ƴϰų� ���� �Ÿ� �̻� �̵��� ��� ��Ŷ ����
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
	if (!Cast<AGPCharacterMyplayer>(this)) return;

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = CharacterInfo.AttackRange;
	const float AttackRadius = CharacterInfo.AttackRadius;

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool bHitDetected = GetWorld()->SweepSingleByChannel(
		OutHitResult, Start, End, FQuat::Identity, CCHANNEL_GPACTION,
		FCollisionShape::MakeSphere(AttackRadius), Params);

	if (bHitDetected && IsValid(OutHitResult.GetActor()))
	{
		AGPCharacterBase* TargetCharacter = Cast<AGPCharacterBase>(OutHitResult.GetActor());
		if (IsValid(TargetCharacter))
		{
			const FInfoData& TargetInfo = TargetCharacter->CharacterInfo;

			const FVector TargetLocation = TargetCharacter->GetActorLocation();
			const float TargetCollisionRadius = TargetInfo.CollisionRadius;
			const float TargetHalfHeight = TargetCollisionRadius;

#if ENABLE_DRAW_DEBUG
			DrawDebugCapsule(GetWorld(), TargetLocation, TargetHalfHeight, TargetCollisionRadius,
				FQuat::Identity, FColor::Yellow, false, 5.f);
#endif
			auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
			if (NetworkMgr)
			{
				NetworkMgr->SendPlayerAttackPacket(TargetCharacter->CharacterInfo.ID);
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	const FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	const float CapsuleHalfHeight = AttackRange * 0.5f;
	const FColor DrawColor = FColor::Red;
	if (bHitDetected)
	{
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius,
			FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
			DrawColor, false, 5.f);
	}
#endif
}

