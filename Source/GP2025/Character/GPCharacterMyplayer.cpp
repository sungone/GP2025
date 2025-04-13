// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GPCharacterMyplayer.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "GPCharacterControlData.h"
#include "InputMappingContext.h"
#include "Physics/GPCollision.h"
#include "Network/GPNetworkManager.h"
#include "Blueprint/UserWidget.h"
#include "Player/GPPlayerController.h"
#include "UI/GPInGameWidget.h"
#include "Character/Modules/GPMyplayerInputHandler.h" 
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/Modules/GPMyplayerNetworkSyncHandler.h"

AGPCharacterMyplayer::AGPCharacterMyplayer()
{
	// Camera 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	CameraBoom->SetupAttachment(RootComponent);
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Character Type
	CurrentCharacterType = (uint8)Type::EPlayer::GUNNER;
}

void AGPCharacterMyplayer::BeginPlay()
{
	Super::BeginPlay();
	NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();

	if (NetMgr)
	{
		NetMgr->SetMyPlayer(Cast<AGPCharacterPlayer>(this));
		NetMgr->OnLoginSuccess.AddDynamic(this, &AGPCharacterMyplayer::OnPlayerLoginSucess);
	}

	// Camera Handler 
	CameraHandler = NewObject<UGPMyplayerCameraHandler>(this, UGPMyplayerCameraHandler::StaticClass());
	if (CameraHandler)
		CameraHandler->Initialize(this);

	// Network Sync Handler
	NetworkSyncHandler = NewObject<UGPMyplayerNetworkSyncHandler>(this, UGPMyplayerNetworkSyncHandler::StaticClass());
	if (NetworkSyncHandler)
		NetworkSyncHandler->Initialize(this);

	SetCharacterType(CurrentCharacterType);
}

void AGPCharacterMyplayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraHandler)
		CameraHandler->Tick(DeltaTime);

	if (!NetMgr) return;

	if (NetworkSyncHandler)
		NetworkSyncHandler->Tick(DeltaTime);
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

	if (NetworkSyncHandler)
	{
		NetworkSyncHandler->WalkSpeed = CharacterControlData->WalkSpeed;
		NetworkSyncHandler->SprintSpeed = CharacterControlData->SprintSpeed;
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


