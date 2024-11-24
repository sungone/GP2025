// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GPCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GPCharacterPlayerControlData.h"
#include "Network/GPGameInstance.h"

AGPCharacterPlayer::AGPCharacterPlayer()
{
	// Camera Setting
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Input Setting
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

	// Character Player Control Setting
	static ConstructorHelpers::FObjectFinder<UGPCharacterPlayerControlData> DefaultDataRef(TEXT("/Script/GP2025.GPCharacterPlayerControlData'/Game/CharacterPlayerControl/GPC_Default.GPC_Default'"));
	if (DefaultDataRef.Object)
	{
		CharacterPlayerControlManager.Add(ECharacterPlayerControlType::Default, DefaultDataRef.Object);
	}
}

void AGPCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterControl(ECharacterPlayerControlType::Default);

	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->MyPlayer = this;
		GameInstance->OtherPlayerClass = AGPCharacterBase::StaticClass();
	}

	LastLocation = GetActorLocation();
	LastRotationYaw = GetActorRotation().Yaw;
	LastSendPlayerInfo = PlayerInfo;
}

void AGPCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance)
		return;

	MovePacketSendTimer -= DeltaTime;
	FVector CurrentLocation = GetActorLocation();
	float CurrentRotationYaw = GetActorRotation().Yaw;

	PlayerInfo.SetLocation(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z);
	PlayerInfo.Yaw = CurrentRotationYaw;
	PlayerInfo.Speed = GetVelocity().Size();

	float DistanceMoved = FVector::DistSquared(CurrentLocation, LastLocation);
	LastLocation = CurrentLocation;

	const float YawThreshold = 10.f;
	bool bYawChanged = (FMath::Abs(CurrentRotationYaw - LastRotationYaw) > YawThreshold);
	LastRotationYaw = CurrentRotationYaw;

	// IDLE 상태인지 아닌지 판단 - IDLE 조건 1 : 이동 거리가 0.5cm 이하
	const float NotMovedThreshold = 0.25f;
	if ( (DistanceMoved >= NotMovedThreshold) )
	{
		PlayerInfo.RemoveState(STATE_IDLE);
	}
	else if ( (DistanceMoved < NotMovedThreshold) )
	{
		PlayerInfo.AddState(STATE_IDLE);
	}

	// Jump() 시 패킷 전송
	if (isJumpStart)
	{
		isJumpStart = false;
		GameInstance->SendPlayerMovePacket();
		LastSendPlayerInfo = PlayerInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Jump Issue"));
		return;
	}

	// 점프 후 착지를 안하고 플레이어가 계속 공중에 떠 있다면 떨어뜨리기 위해 패킷 전송
	if (PlayerInfo.HasState(STATE_IDLE) && !PlayerInfo.HasState(STATE_JUMP) && LastSendPlayerInfo.Z > 120.f)
	{
		PlayerInfo.Z = GroundZLocation;

		if (LastSendPlayerInfo.HasState(STATE_RUN))
		{
			PlayerInfo.Speed = SprintSpeed;
		}
		else
		{
			PlayerInfo.Speed = WalkSpeed;
		}

		GameInstance->SendPlayerMovePacket();
		LastSendPlayerInfo = PlayerInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Air Fixed Issue"));
		return;
	}

	// IDLE 상태에서 캐릭터의 회전이 변경되었을 때 패킷 전송
	if (bYawChanged && PlayerInfo.HasState(STATE_IDLE))
	{
		GameInstance->SendPlayerMovePacket();
		LastSendPlayerInfo = PlayerInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Rotation Issue"));
		return;
	}

	// 일정 시간마다 서버에 패킷 전송
	if (MovePacketSendTimer <= 0)
	{
		MovePacketSendTimer = PACKETSENDTIME;

		if ((!PlayerInfo.HasState(STATE_IDLE)) || (DistanceMoved >= NotMovedThreshold))
		{
			GameInstance->SendPlayerMovePacket();
			LastSendPlayerInfo = PlayerInfo;
			UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : periodically"));
		}
	}
	else 
	{
		if ((PlayerInfo.HasState(STATE_IDLE)) && (DistanceMoved >= NotMovedThreshold))
		{
			GameInstance->SendPlayerMovePacket();
			MovePacketSendTimer = PACKETSENDTIME;
			LastSendPlayerInfo = PlayerInfo;
			UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : I am Idle but when I moved"));
		}
	}
}

void AGPCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGPCharacterPlayer::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGPCharacterPlayer::StopJumping);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGPCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGPCharacterPlayer::Look);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AGPCharacterPlayer::StartSprinting);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGPCharacterPlayer::StopSprinting);
	EnhancedInputComponent->BindAction(AutoAttackAction, ETriggerEvent::Triggered, this, &AGPCharacterPlayer::AutoAttack);
}

void AGPCharacterPlayer::SetCharacterControl(ECharacterPlayerControlType NewCharacterPlayerControlType)
{
	UGPCharacterPlayerControlData* NewCharacterPlayerControl = CharacterPlayerControlManager[NewCharacterPlayerControlType];
	check(NewCharacterPlayerControl);

	SetCharacterControlData(NewCharacterPlayerControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterPlayerControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}
}

void AGPCharacterPlayer::SetCharacterControlData(const UGPCharacterPlayerControlData* CharacterPlayerControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterPlayerControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterPlayerControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterPlayerControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterPlayerControlData->RotationRate;
}

void AGPCharacterPlayer::Move(const FInputActionValue& Value)
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

void AGPCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGPCharacterPlayer::Jump()
{
	Super::Jump();
	PlayerInfo.RemoveState(STATE_IDLE);
	PlayerInfo.AddState(STATE_JUMP);
	isJumpStart = true;
}

void AGPCharacterPlayer::StopJumping()
{
	Super::StopJumping();
	PlayerInfo.RemoveState(STATE_JUMP);
}

void AGPCharacterPlayer::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	PlayerInfo.AddState(STATE_RUN);
}

void AGPCharacterPlayer::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	PlayerInfo.RemoveState(STATE_RUN);
}

void AGPCharacterPlayer::AutoAttack()
{
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance)
		return;

	if (bIsAutoAttacking == false && !PlayerInfo.HasState(STATE_AUTOATTACK))
	{
		PlayerInfo.AddState(STATE_AUTOATTACK);
		GameInstance->sendPlayerAttackPacket();
	}

	ProcessAutoAttackCommand();
}
