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
#include "Character/Modules/GPMyplayerInputHandler.h" 
#include "Character/Modules/GPMyplayerUIManager.h"

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

	// 기본 캐릭터 타입
	CurrentCharacterType = (uint8)Type::EPlayer::GUNNER;
}

//void AGPCharacterMyplayer::OnPlayerLoginSucess()
//{
//	if (InventoryWidgetClass)
//	{
//		InventoryWidget = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);
//	}
//
//	if (SettingWidgetClass)
//	{
//		SettingWidget = CreateWidget<UUserWidget>(GetWorld(), SettingWidgetClass);
//	}
//
//	if (InGameWidgetClass)
//	{
//		InGameWidget = CreateWidget<UUserWidget>(GetWorld(), InGameWidgetClass);
//		if (InGameWidget)
//		{
//			InGameWidget->AddToViewport();
//			APlayerController* PC = Cast<AGPPlayerController>(GetController());
//			if (PC)
//			{
//				PC->SetShowMouseCursor(false);
//				PC->SetInputMode(FInputModeGameOnly());
//				InGameWidget->SetVisibility(ESlateVisibility::Visible);
//			}
//		}
//	}
//
//	if (GunCrosshairWidgetClass)
//	{
//		GunCrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), GunCrosshairWidgetClass);
//		if (GunCrosshairWidget)
//		{
//			GunCrosshairWidget->AddToViewport();
//			GunCrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
//		}
//	}
//}

void AGPCharacterMyplayer::BeginPlay()
{
	Super::BeginPlay();
	NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	SetCharacterType(CurrentCharacterType);

	if (NetMgr)
	{
		NetMgr->SetMyPlayer(Cast<AGPCharacterPlayer>(this));
		NetMgr->OnLoginSuccess.AddDynamic(this, &AGPCharacterMyplayer::OnPlayerLoginSucess);
	}
	LastLocation = GetActorLocation();
	LastRotationYaw = GetActorRotation().Yaw;
	LastSendPlayerInfo = CharacterInfo;

	// 조준 카메라 뷰 설정
	if (FollowCamera)
	{
		DefaultFOV = FollowCamera->FieldOfView;
	}

	DefaultCameraOffset = CameraBoom->GetRelativeLocation();
	DefaultArmLength = CameraBoom->TargetArmLength;
}

void AGPCharacterMyplayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 카메라 줌인 줌아웃 뷰 설정
	{
		if (FollowCamera)
		{
			const float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
			const float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
			FollowCamera->SetFieldOfView(NewFOV);
		}

		if (CameraBoom)
		{
			float TargetArmLength = bWantsToZoom ? ZoomedArmLength : DefaultArmLength;
			FVector TargetOffset = bWantsToZoom ? ZoomedCameraOffset : DefaultCameraOffset;

			float InterpSpeed = 10.f;
			float Delta = DeltaTime;

			float NewArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetArmLength, Delta, InterpSpeed);
			FVector NewOffset = FMath::VInterpTo(CameraBoom->GetRelativeLocation(), TargetOffset, Delta, InterpSpeed);

			CameraBoom->TargetArmLength = NewArmLength;
			CameraBoom->SetRelativeLocation(NewOffset);
		}

		if (bWantsToZoom)
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
			bUseControllerRotationYaw = true;
		}
		else
		{
			GetCharacterMovement()->bOrientRotationToMovement = true;
			bUseControllerRotationYaw = false;
		}
	}

	if (!NetMgr) return;

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
		NetMgr->SendPlayerMovePacket();
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

		NetMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = CharacterInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Air Fixed Issue"));
		return;
	}

	// IDLE 상태에서 캐릭터의 회전이 변경되었을 때 패킷 전송
	if (bYawChanged && CharacterInfo.HasState(STATE_IDLE))
	{
		NetMgr->SendPlayerMovePacket();
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
			NetMgr->SendPlayerMovePacket();
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

	if (!InputHandler)
	{
		InputHandler = NewObject<UGPMyplayerInputHandler>(this, UGPMyplayerInputHandler::StaticClass());
		InputHandler->Initialize(this, EnhancedInputComponent);
	}
}

void AGPCharacterMyplayer::SetCharacterType(ECharacterType NewCharacterType)
{
	Super::SetCharacterType(NewCharacterType);

	UGPCharacterControlData* NewCharacterData = CharacterTypeManager[NewCharacterType];
	check(NewCharacterData);
	SetCharacterData(NewCharacterData);

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

void AGPCharacterMyplayer::OnPlayerLoginSucess()
{
	if (!UIManager)
	{
		UIManager = NewObject<UGPMyplayerUIManager>(this, UGPMyplayerUIManager::StaticClass());
		if (UIManager)
		{
			UIManager->Initialize(this);
			UIManager->OnLoginCreateWidget();
		}
	}
}

bool AGPCharacterMyplayer::bIsGunnerCharacter() const
{
	return (CurrentCharacterType == (uint8)Type::EPlayer::GUNNER);
}

void AGPCharacterMyplayer::ChangePlayerType()
{
	if (bIsGunnerCharacter())
	{
		CurrentCharacterType = static_cast<uint8>(Type::EPlayer::WARRIOR);
	}
	else
	{
		CurrentCharacterType = static_cast<uint8>(Type::EPlayer::GUNNER);
	}

	SetCharacterType(static_cast<ECharacterType>(CurrentCharacterType));
}

void AGPCharacterMyplayer::SetCharacterInfo(FInfoData& CharacterInfo_)
{
	Super::SetCharacterInfo(CharacterInfo_);

	UGPInGameWidget* InGame = Cast<UGPInGameWidget>(UIManager->GetInGameWidget());

	if (!InGame) return;

	InGame->UpdateHealthBar(CharacterInfo_.Stats.Hp / CharacterInfo_.Stats.MaxHp);
	InGame->UpdateExpBar(CharacterInfo_.Stats.Exp / CharacterInfo_.Stats.MaxExp);
	InGame->UpdatePlayerLevel(CharacterInfo_.Stats.Level);
}


