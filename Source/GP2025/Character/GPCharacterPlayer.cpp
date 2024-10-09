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
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 800.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/DokkaebiAssets/boss_body_bip.boss_body_bip'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animation/PlayerAnimation/ABP_GPCharacterPlayer.ABP_GPCharacterPlayer_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

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



// WASD 이동 구현 로직
void AGPCharacterPlayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (MovementVector.IsNearlyZero())  // 입력이 없으면 아무 동작도 하지 않음
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);

	// 현재 위치
	FVector CurrentLocation = GetActorLocation();
	float DistanceMoved = FVector::Dist(CurrentLocation, PreviousLocation);

	// 일정 거리 이상 이동한 경우에만 패킷을 전송
	if (DistanceMoved > 10.0f)  // 10 유닛 이상 이동했을 때만 서버로 전송
	{
		bool bIsCurrentlyJumping = GetCharacterMovement()->IsFalling();

		if (bIsCurrentlyJumping != bWasJumping)
		{
			if (UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance()))
			{
				GameInstance->SendPlayerMovePacket(CurrentLocation, GetActorRotation(), bIsCurrentlyJumping);
			}
			bWasJumping = bIsCurrentlyJumping;
		}

		// 이전 위치를 업데이트
		PreviousLocation = CurrentLocation;
	}
}

// 마우스 화면 전환 로직
void AGPCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AGPCharacterPlayer::Jump()
{
	Super::Jump();

	if (UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance()))
	{
		FVector CurrentLocation = GetActorLocation();
		FRotator CurrentRotation = GetActorRotation();
		GameInstance->SendPlayerMovePacket(CurrentLocation, CurrentRotation , true);
	}
}

void AGPCharacterPlayer::StopJumping()
{
	Super::StopJumping();

	if (UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance()))
	{
		FVector CurrentLocation = GetActorLocation();
		FRotator CurrentRotation = GetActorRotation();
		GameInstance->SendPlayerMovePacket(CurrentLocation, CurrentRotation, false);
	}
}

void AGPCharacterPlayer::StartSprinting()
{
	UE_LOG(LogTemp, Warning, TEXT("StartSprinting called"));
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AGPCharacterPlayer::StopSprinting()
{
	UE_LOG(LogTemp, Warning, TEXT("StopSprinting called"));
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
