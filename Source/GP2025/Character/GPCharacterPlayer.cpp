// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GPCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GPCharacterControlData.h"
#include "Network/GPGameInstance.h"
#include "GPCharacterNonPlayer.h"
#include "Inventory/GPInventoryMoney.h"
#include "CharacterStat/GPCharacterStatComponent.h"

AGPCharacterPlayer::AGPCharacterPlayer()
{
	// ī�޶� ����
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
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

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionChangeCharacterTypeRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PlayerInput/Actions/IA_ChangeCharacterType.IA_ChangeCharacterType'"));
	if (InputActionChangeCharacterTypeRef.Object)
	{
		ChangeCharacterTypeAction = InputActionChangeCharacterTypeRef.Object;
	}

	// �⺻ ĳ���� Ÿ���� ���� ĳ���ͷ�
	CurrentCharacterType = ECharacterType::P_WARRIOR;

	// �浹 �Լ� ���ε�
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AGPCharacterPlayer::OnCapsuleBeginOverlap);
}

void AGPCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->SetWorldScale3D(FVector(1.0f));
	SetCharacterType(CurrentCharacterType);
	EquipItemFromDataAsset(CharacterTypeManager[CurrentCharacterType]);

	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->MyPlayer = this;
		GameInstance->OtherPlayerClass = AGPCharacterBase::StaticClass();
		GameInstance->MonsterClass = AGPCharacterBase::StaticClass();
	}

	LastLocation = GetActorLocation();
	LastRotationYaw = GetActorRotation().Yaw;
	LastSendPlayerInfo = CharacterInfo;
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
	if ( (DistanceMoved >= NotMovedThreshold) )
	{
		CharacterInfo.RemoveState(STATE_IDLE);
	}
	else if ( (DistanceMoved < NotMovedThreshold) )
	{
		CharacterInfo.AddState(STATE_IDLE);
	}

	// Jump() �� ��Ŷ ����
	if (isJumpStart && !bWasJumping)
	{
		isJumpStart = false;
		bWasJumping = true;
		GameInstance->SendPlayerMovePacket();
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
		&& (LastSendPlayerInfo.Z - GroundZLocation) > AirThreshold)
	{
		CharacterInfo.Z = GroundZLocation;
		CharacterInfo.Speed = LastSendPlayerInfo.HasState(STATE_RUN) ? SprintSpeed : WalkSpeed;

		GameInstance->SendPlayerMovePacket();
		LastSendPlayerInfo = CharacterInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Air Fixed Issue"));
		return;
	}

	// IDLE ���¿��� ĳ������ ȸ���� ����Ǿ��� �� ��Ŷ ����
	if (bYawChanged && CharacterInfo.HasState(STATE_IDLE))
	{
		GameInstance->SendPlayerMovePacket();
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
			GameInstance->SendPlayerMovePacket();
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
	EnhancedInputComponent->BindAction(ChangeCharacterTypeAction, ETriggerEvent::Triggered, this, &AGPCharacterPlayer::ChangeCharacterControl);
}

bool bCanChangeCharacterControl = true;
void AGPCharacterPlayer::ChangeCharacterControl()
{
	if (!bCanChangeCharacterControl)
		return;
	bCanChangeCharacterControl = false;

	if (CurrentCharacterType == ECharacterType::P_WARRIOR)
	{
		SetCharacterType(ECharacterType::P_GUNNER);
		UE_LOG(LogTemp, Log, TEXT("Change Gunner Control Type."));
	}
	else if (CurrentCharacterType == ECharacterType::P_GUNNER)
	{
		SetCharacterType(ECharacterType::P_WARRIOR);
		UE_LOG(LogTemp, Log, TEXT("Change Warrior Control Type."));
	}

	// ���� �ð� �� �÷��׸� �ٽ� Ȱ��ȭ
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			bCanChangeCharacterControl = true;
		});
}

void AGPCharacterPlayer::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ���� �Ծ��� ��(�浹���� ��) �޼��� ���
	AGPInventoryMoney* Money = Cast<AGPInventoryMoney>(OtherActor);
	if (Money)
	{
		UE_LOG(LogTemp, Log, TEXT("We found money"));
	}
}

void AGPCharacterPlayer::SetCharacterType(ECharacterType NewCharacterType)
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

void AGPCharacterPlayer::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
	Super::SetCharacterData(CharacterControlData);

	WalkSpeed = CharacterControlData->WalkSpeed;
	SprintSpeed = CharacterControlData->SprintSpeed;
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
	isJumpStart = true;
	CharacterInfo.RemoveState(STATE_IDLE);
	CharacterInfo.AddState(STATE_JUMP);
}

void AGPCharacterPlayer::StopJumping()
{
	Super::StopJumping();
	CharacterInfo.RemoveState(STATE_JUMP);
}

void AGPCharacterPlayer::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	CharacterInfo.AddState(STATE_RUN);
}

void AGPCharacterPlayer::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	CharacterInfo.RemoveState(STATE_RUN);
}

void AGPCharacterPlayer::AutoAttack()
{
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance)
		return;

	if (bIsAutoAttacking == false && !CharacterInfo.HasState(STATE_AUTOATTACK))
	{
		CharacterInfo.AddState(STATE_AUTOATTACK);
		GameInstance->SendPlayerAttackPacket();
	}

	ProcessAutoAttackCommand();
}
