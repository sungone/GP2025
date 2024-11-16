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
	// Camera 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Input
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
	if (UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance()))
	{
		GameInstance->MyPlayer = this;
		GameInstance->OtherPlayerClass = AGPCharacterBase::StaticClass();
	}

	LastLocation = GetActorLocation();
	LastSendPlayerInfo = PlayerInfo;
}

void AGPCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance)
		return;

	// ���� ��ġ ��������
	FVector CurrentLocation = GetActorLocation();
	float CurrentYaw = GetActorRotation().Yaw;

	if (PlayerInfo.HasState(STATE_WALK))
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Location (%f,%f,%f)"),
			CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z);
	}

	// ��ġ ��ȭ�� ���
	float DistanceMoved = FVector::DistSquared(CurrentLocation, LastLocation);

	const float DistThreshold = 30.f;
	const float YawThreshold = 0.1f;
	bool bYawChanged = (FMath::Abs(CurrentYaw - PlayerInfo.Yaw) > YawThreshold);

	// ������ ���� ���� (�Ӱ谪 0.01f�� �������� ����)
	if (DistanceMoved < 0.1f)  // �������� ���� ���� ��
	{
		PlayerInfo.RemoveState(STATE_WALK);
		PlayerInfo.AddState(STATE_IDLE);
	}
	else  // �������� ���� ��
	{
		PlayerInfo.RemoveState(STATE_IDLE);
		PlayerInfo.AddState(STATE_WALK);
	}

	if (PlayerInfo.HasState(STATE_WALK))
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterPlayer State %d"),
			PlayerInfo.State);
	}

	LastLocation = CurrentLocation;
	PlayerInfo.Speed = GetVelocity().Size2D();

	if (bYawChanged)
	{
		PlayerInfo.Yaw = CurrentYaw;  // ���� Yaw ������ ������Ʈ
	}

	if (DistanceMoved > DistThreshold)
	{
		PlayerInfo.X = CurrentLocation.X;
		PlayerInfo.Y = CurrentLocation.Y;
		PlayerInfo.Z = CurrentLocation.Z;
	}

	// 1�ʸ��� �÷��̾� �̵� ��Ŷ�� ������ ����
	MovePacketSendTimer -= DeltaTime;

	if (MovePacketSendTimer <= 0)
	{
		MovePacketSendTimer = 0.2;

		if (!PlayerInfo.HasState(STATE_IDLE) || bYawChanged || DistanceMoved > DistThreshold)
		{
			GameInstance->SendPlayerMovePacket();
		}

		if (PlayerInfo.HasState(STATE_IDLE) && CurrentLocation.Z > 120.f)
		{
			PlayerInfo.Z = 115.7f;
			GameInstance->SendPlayerMovePacket();
		}

		LastSendPlayerInfo = PlayerInfo;
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

		FVector DesiredMovementDirection = (ForwardDirection * MovementVector.X) + (RightDirection * MovementVector.Y);
		DesiredMovementDirection.Z = 0;
		FRotator DesiredRotation = DesiredMovementDirection.Rotation();
		PlayerInfo.Yaw = DesiredRotation.Yaw;

		FVector CurrentLocation = GetActorLocation();
		PlayerInfo.SetVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z);
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
	PlayerInfo.AddState(STATE_JUMP);
}

void AGPCharacterPlayer::StopJumping()
{
	Super::StopJumping();
	PlayerInfo.RemoveState(STATE_JUMP);
}

void AGPCharacterPlayer::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	PlayerInfo.RemoveState(STATE_WALK);
	PlayerInfo.AddState(STATE_RUN);
}

void AGPCharacterPlayer::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	PlayerInfo.RemoveState(STATE_RUN);
	PlayerInfo.AddState(STATE_WALK);
}
