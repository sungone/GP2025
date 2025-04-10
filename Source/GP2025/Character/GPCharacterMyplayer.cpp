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
	// ī�޶� ����
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// �⺻ ĳ���� Ÿ��
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

	// ���� ī�޶� �� ����
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

	// ī�޶� ���� �ܾƿ� �� ����
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
		NetMgr->SendPlayerMovePacket();
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
		CharacterInfo.Stats.Speed = LastSendPlayerInfo.HasState(STATE_RUN) ? SprintSpeed : WalkSpeed;

		NetMgr->SendPlayerMovePacket();
		LastSendPlayerInfo = CharacterInfo;
		UE_LOG(LogTemp, Log, TEXT("Character Player Send Packet To Server : Air Fixed Issue"));
		return;
	}

	// IDLE ���¿��� ĳ������ ȸ���� ����Ǿ��� �� ��Ŷ ����
	if (bYawChanged && CharacterInfo.HasState(STATE_IDLE))
	{
		NetMgr->SendPlayerMovePacket();
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


