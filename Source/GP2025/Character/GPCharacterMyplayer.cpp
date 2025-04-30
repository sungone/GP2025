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
#include "Components/ProgressBar.h"
#include "Character/Modules/GPMyplayerInputHandler.h" 
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/Modules/GPMyplayerNetworkSyncHandler.h"
#include "Skill/GPSkillCoolDownHandler.h"
#include "Components/TextBlock.h"
#include "GPCharacterMyplayer.h"

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
		NetMgr->OnEnterGame.AddDynamic(this, &AGPCharacterMyplayer::OnPlayerEnterGame);
		NetMgr->OnEnterLobby.AddDynamic(this, &AGPCharacterMyplayer::OnPlayerEnterLobby);
	}

	// Camera Handler 
	CameraHandler = NewObject<UGPMyplayerCameraHandler>(this, UGPMyplayerCameraHandler::StaticClass());
	if (CameraHandler)
		CameraHandler->Initialize(this);
	CameraHandler->AddToRoot();

	// Skill Cool Down Handler
	SkillCoolDownHandler = NewObject<UGPSkillCoolDownHandler>(this, UGPSkillCoolDownHandler::StaticClass());
	if (SkillCoolDownHandler)
		SkillCoolDownHandler->Init(this);
	SkillCoolDownHandler->AddToRoot();

	// Network Sync Handler
	NetworkSyncHandler = NewObject<UGPMyplayerNetworkSyncHandler>(this, UGPMyplayerNetworkSyncHandler::StaticClass());
	if (NetworkSyncHandler)
		NetworkSyncHandler->Initialize(this);
	NetworkSyncHandler->AddToRoot();

	UIManager = NewObject<UGPMyplayerUIManager>(this, UGPMyplayerUIManager::StaticClass());
	if (UIManager)
	{
		UIManager->Initialize(this);
		UIManager->ShowLoginUI();
	}
	
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

	if (!SkillCoolDownHandler || !IsValid(SkillCoolDownHandler))
		return;
	UpdateSkillCooldownBars();
}

void AGPCharacterMyplayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (SkillCoolDownHandler)
	{
		if (SkillCoolDownHandler->IsRooted())
		{
			SkillCoolDownHandler->RemoveFromRoot();
			UE_LOG(LogTemp, Warning, TEXT("SkillCoolDownHandler RemoveFromRoot() called in EndPlay."));
		}

		SkillCoolDownHandler = nullptr;
	}

	if (CameraHandler)
	{
		if (CameraHandler->IsRooted())
		{
			CameraHandler->RemoveFromRoot();
			UE_LOG(LogTemp, Warning, TEXT("CameraHandler RemoveFromRoot() called in EndPlay."));
		}
		CameraHandler = nullptr;
	}

	if (NetworkSyncHandler)
	{
		if (NetworkSyncHandler->IsRooted())
		{
			NetworkSyncHandler->RemoveFromRoot();
			UE_LOG(LogTemp, Warning, TEXT("NetworkSyncHandler RemoveFromRoot() called in EndPlay."));
		}
		NetworkSyncHandler = nullptr;
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

void AGPCharacterMyplayer::OnPlayerEnterGame()
{
	if (UIManager)
	{
		if (UIManager->LobbyWidget)
		{
			if (UIManager->LobbyWidget->IsInViewport())
			{
				UIManager->LobbyWidget->RemoveFromParent();
			}
		}

		UIManager->OnSetUpInGameWidgets();
	}

	UE_LOG(LogTemp, Log, TEXT("[UI] faefefsafsf "));
}

void AGPCharacterMyplayer::OnPlayerEnterLobby()
{
	if (UIManager)
	{
		if (UIManager->LoginWidget)
		{
			if (UIManager->LoginWidget->IsInViewport())
			{
				UIManager->LoginWidget->RemoveFromParent();
			}

		}

		UIManager->ShowLobbyUI();
	}

	UE_LOG(LogTemp, Log, TEXT("[UI] ddddddddd "));
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

float AGPCharacterMyplayer::GetSkillCooldownRatio(ESkillGroup SkillGroup)
{
	if (!SkillCoolDownHandler)
	{
		return 0.f;
	}

	int32 SkillLevel = CharacterInfo.GetSkillLevel(SkillGroup);

	if (SkillLevel <= 0)
	{
		return 1.f;
	}

	float TotalCooldown = SkillCoolDownHandler->GetTotalCooldownTime(SkillGroup, SkillLevel);
	float RemainingCooldown = SkillCoolDownHandler->GetRemainingCooldownTime(SkillGroup, SkillLevel);

	if (TotalCooldown <= 0.f)
	{
		return 0.f;
	}

	return FMath::Clamp(RemainingCooldown / TotalCooldown, 0.f, 1.f);
}

void AGPCharacterMyplayer::UpdateSkillCooldownBars()
{
	if (!UIManager || !UIManager->GetInGameWidget()
		|| !UIManager->GetInGameWidget()->QSkillBar
		|| !UIManager->GetInGameWidget()->ESkillBar
		|| !UIManager->GetInGameWidget()->RSkillBar)
		return;

	if (!SkillCoolDownHandler || SkillCoolDownHandler->SkillCooldownTimes.Num() == 0)
	{
		return;
	}

	auto* QSkill = CharacterInfo.GetSkillData(bIsGunnerCharacter() ? ESkillGroup::Throwing : ESkillGroup::HitHard);
	if (QSkill && QSkill->IsValid())
	{
		UIManager->GetInGameWidget()->QSkillBar->SetPercent(1 - GetSkillCooldownRatio(QSkill->SkillGID));
	}
	else
	{
		UIManager->GetInGameWidget()->QSkillBar->SetPercent(0.f);
	}
	UIManager->GetInGameWidget()->QSkillText->SetColorAndOpacity(UIManager->GetInGameWidget()->GetQSkillTextColor());

	auto* ESkill = CharacterInfo.GetSkillData(bIsGunnerCharacter() ? ESkillGroup::FThrowing : ESkillGroup::Clash);
	if (ESkill && ESkill->IsValid())
	{
		UIManager->GetInGameWidget()->ESkillBar->SetPercent(1 - GetSkillCooldownRatio(ESkill->SkillGID));
	}
	else
	{
		UIManager->GetInGameWidget()->ESkillBar->SetPercent(0.f);
	}
	UIManager->GetInGameWidget()->ESkillText->SetColorAndOpacity(UIManager->GetInGameWidget()->GetESkillTextColor());

	auto* RSkill = CharacterInfo.GetSkillData(bIsGunnerCharacter() ? ESkillGroup::Anger : ESkillGroup::Whirlwind);
	if (RSkill && RSkill->IsValid())
	{
		UIManager->GetInGameWidget()->RSkillBar->SetPercent(1 - GetSkillCooldownRatio(RSkill->SkillGID));
	}
	else
	{
		UIManager->GetInGameWidget()->RSkillBar->SetPercent(0.f);
	}
	UIManager->GetInGameWidget()->RSkillText->SetColorAndOpacity(UIManager->GetInGameWidget()->GetRSkillTextColor());
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


