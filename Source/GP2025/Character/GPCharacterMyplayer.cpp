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
#include "Inventory/GPInventory.h"
#include "Components/ProgressBar.h"
#include "Character/Modules/GPMyplayerInputHandler.h" 
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/Modules/GPMyplayerNetworkSyncHandler.h"
#include "Character/Modules/GPMyplayerSoundManager.h"
#include "Skill/GPSkillCoolDownHandler.h"
#include "UI/GPQuestWidget.h"
#include "Components/TextBlock.h"
#include "kismet/GameplayStatics.h"
#include "GPCharacterMyplayer.h"

AGPCharacterMyplayer::AGPCharacterMyplayer()
{
	// Camera 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	CameraBoom->SetupAttachment(RootComponent);
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Character Type
	CurrentCharacterType = static_cast<uint8>(Type::EPlayer::GUNNER);
}

void AGPCharacterMyplayer::BeginPlay()
{
	Super::BeginPlay();
	NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();

	if (NetMgr)
	{
		NetMgr->SetMyPlayer(this);
		NetMgr->OnEnterGame.AddDynamic(this, &AGPCharacterMyplayer::OnPlayerEnterGame);
		NetMgr->OnEnterLobby.AddDynamic(this, &AGPCharacterMyplayer::OnPlayerEnterLobby);
	}

	// Sound Handler
	SoundManager = NewObject<UGPMyplayerSoundManager>(this, UGPMyplayerSoundManager::StaticClass());
	if (SoundManager)
	{
		SoundManager->Initialize(this);
	}

	// Camera Handler 
	CameraHandler = NewObject<UGPMyplayerCameraHandler>(this, UGPMyplayerCameraHandler::StaticClass());
	if (CameraHandler)
	{
		CameraHandler->Initialize(this);
		CameraHandler->ConfigureCameraCollision();
	}

	// Skill Cool Down Handler
	SkillCoolDownHandler = NewObject<UGPSkillCoolDownHandler>(this, UGPSkillCoolDownHandler::StaticClass());
	if (SkillCoolDownHandler)
		SkillCoolDownHandler->Init(this);


	// Network Sync Handler
	NetworkSyncHandler = NewObject<UGPMyplayerNetworkSyncHandler>(this, UGPMyplayerNetworkSyncHandler::StaticClass());
	if (NetworkSyncHandler)
		NetworkSyncHandler->Initialize(this);


	// UIManager and Show Login UI
	UIManager = NewObject<UGPMyplayerUIManager>(this, UGPMyplayerUIManager::StaticClass());
	if (UIManager)
	{
		UIManager->Initialize(this);
		UIManager->ShowLoginUI();
	}
	
	SetCharacterType(CurrentCharacterType);

	// 몬스터와 충돌 설정
	USkeletalMeshComponent* MyMesh = GetMesh();
	MyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MyMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	MyMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	MyMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	MyMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AGPCharacterMyplayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraHandler)
		CameraHandler->Tick(DeltaTime);

	if (!NetMgr) return;

	if (NetworkSyncHandler)
		NetworkSyncHandler->Tick(DeltaTime);

	if (SkillCoolDownHandler && IsValid(SkillCoolDownHandler))
	{
		UpdateSkillCooldownBars();
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

	// LoginSound 중지 -> 현재 레벨에 맞는 Background Sound 재생
	if (SoundManager)
	{
		SoundManager->StopBGM();             
		SoundManager->PlayBGMForCurrentLevel(); 
	}

	if (CharacterInfo.GetCurrentQuest().Type == QuestType::TUT_START
		|| CharacterInfo.GetCurrentQuest().Type == QuestType::NONE)
	{

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this]()
				{
					UE_LOG(LogTemp, Log, TEXT("[MyPlayer] Executing tutorial quest widget."));

					if (UIManager)
					{
						UIManager->PlayTutorialQuestWidget();
					}
				}),
			1.0f,
			false);
	}
}

void AGPCharacterMyplayer::OnPlayerEnterLobby()
{
	if (UIManager)
	{
		if (UIManager->LoginWidget && UIManager->LoginWidget->IsInViewport())
		{
			UIManager->LoginWidget->SetVisibility(ESlateVisibility::Collapsed);
			UIManager->LoginWidget->RemoveFromParent();
		}

		UIManager->ShowLobbyUI();
	}
}

void AGPCharacterMyplayer::SetCharacterType(ECharacterType NewCharacterType)
{
	Super::SetCharacterType(NewCharacterType);

	UGPCharacterControlData** FoundData = CharacterTypeManager.Find(NewCharacterType);
	if (!FoundData || !(*FoundData))
	{
		UE_LOG(LogTemp, Error, TEXT("[SetCharacterType] Not Found CharacterTypeManager : %d "), NewCharacterType);
		return;
	}

	UGPCharacterControlData* NewCharacterData = *FoundData;

	//UGPCharacterControlData* NewCharacterData = CharacterTypeManager[NewCharacterType];
	//check(NewCharacterData);
	SetCharacterData(NewCharacterData);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
#if PLATFORM_ANDROID
		UInputMappingContext* NewMappingContext = NewCharacterData->MobileMappingContext;
#else
		UInputMappingContext* NewMappingContext = NewCharacterData->InputMappingContext;
#endif

		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
		else
		{
#if PLATFORM_ANDROID
			UE_LOG(LogTemp, Error, TEXT("[SetCharacterType] MobileInputMappingContext is NULL"));
#else
			UE_LOG(LogTemp, Error, TEXT("[SetCharacterType] InputMappingContext is NULL"));
#endif
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
	{
		UE_LOG(LogTemp, Error, TEXT("[CooldownUI] Widget references are invalid."));
		return;
	}

	if (!SkillCoolDownHandler || SkillCoolDownHandler->SkillCooldownTimes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[CooldownUI] SkillCoolDownHandler invalid or not initialized."));
		return;
	}

	// Q Skill
	auto* QSkill = CharacterInfo.GetSkillData(bIsGunnerCharacter() ? ESkillGroup::Throwing : ESkillGroup::HitHard);
	if (QSkill && QSkill->IsValid())
	{
		float Ratio = GetSkillCooldownRatio(QSkill->SkillGID);
		UIManager->GetInGameWidget()->QSkillBar->SetPercent(1 - Ratio);
		//UE_LOG(LogTemp, Warning, TEXT("[CooldownUI] QSkill Group=%d, Ratio=%.2f, Bar=%.2f"),
		//	(int32)QSkill->SkillGID, Ratio, 1 - Ratio);
	}
	else
	{
		UIManager->GetInGameWidget()->QSkillBar->SetPercent(0.f);
		//UE_LOG(LogTemp, Warning, TEXT("[CooldownUI] QSkill is invalid."));
	}
	UIManager->GetInGameWidget()->QSkillText->SetColorAndOpacity(UIManager->GetInGameWidget()->GetQSkillTextColor());

	// E Skill
	auto* ESkill = CharacterInfo.GetSkillData(bIsGunnerCharacter() ? ESkillGroup::FThrowing : ESkillGroup::Clash);
	if (ESkill && ESkill->IsValid())
	{
		float Ratio = GetSkillCooldownRatio(ESkill->SkillGID);
		UIManager->GetInGameWidget()->ESkillBar->SetPercent(1 - Ratio);
		//UE_LOG(LogTemp, Warning, TEXT("[CooldownUI] ESkill Group=%d, Ratio=%.2f, Bar=%.2f"),
		//	(int32)ESkill->SkillGID, Ratio, 1 - Ratio);
	}
	else
	{
		UIManager->GetInGameWidget()->ESkillBar->SetPercent(0.f);
		//UE_LOG(LogTemp, Warning, TEXT("[CooldownUI] ESkill is invalid."));
	}
	UIManager->GetInGameWidget()->ESkillText->SetColorAndOpacity(UIManager->GetInGameWidget()->GetESkillTextColor());

	// R Skill
	auto* RSkill = CharacterInfo.GetSkillData(bIsGunnerCharacter() ? ESkillGroup::Anger : ESkillGroup::Whirlwind);
	if (RSkill && RSkill->IsValid())
	{
		float Ratio = GetSkillCooldownRatio(RSkill->SkillGID);
		UIManager->GetInGameWidget()->RSkillBar->SetPercent(1 - Ratio);
		//UE_LOG(LogTemp, Warning, TEXT("[CooldownUI] RSkill Group=%d, Ratio=%.2f, Bar=%.2f"),
		//	(int32)RSkill->SkillGID, Ratio, 1 - Ratio);
	}
	else
	{
		UIManager->GetInGameWidget()->RSkillBar->SetPercent(0.f);
		//UE_LOG(LogTemp, Warning, TEXT("[CooldownUI] RSkill is invalid."));
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

void AGPCharacterMyplayer::SetCharacterInfo(const FInfoData& CharacterInfo_)
{
	Super::SetCharacterInfo(CharacterInfo_);

	UGPInGameWidget* InGame = Cast<UGPInGameWidget>(UIManager->GetInGameWidget());

	if (!InGame) return;

	InGame->UpdateHealthBar(CharacterInfo_.Stats.Hp / CharacterInfo_.Stats.MaxHp);
	InGame->UpdateExpBar(CharacterInfo_.Stats.Exp / CharacterInfo_.Stats.MaxExp);
	InGame->UpdatePlayerLevel(CharacterInfo_.Stats.Level);

	if (CharacterInfo_.GetLevel() > PrevLevel)
	{
		// UIManager->SpawnSkillLevelText(CharacterInfo_.Stats.Level);
		PrevLevel = CharacterInfo_.Stats.Level;

		if (SoundManager && SoundManager->LevelUpSound)
		{
			SoundManager->PlaySFX(SoundManager->LevelUpSound);
		}
	}

	if (UIManager && UIManager->GetInventoryWidget())
	{
		UIManager->GetInventoryWidget()->HandlePlayerStatUpdate();
	}
}

void AGPCharacterMyplayer::PlayFadeOut()
{
	if (UIManager && UIManager->GetInGameWidget())
	{
		UIManager->GetInGameWidget()->PlayFadeOut();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayFadeOut] InGameWidget is not valid."));
	}
}

void AGPCharacterMyplayer::PlayFadeIn()
{
	if (UIManager && UIManager->GetInGameWidget())
	{
		UIManager->GetInGameWidget()->PlayFadeIn();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayFadeOut] InGameWidget is not valid."));
	}
}

void AGPCharacterMyplayer::SetDead(bool bDead)
{
	if (bDead)
	{
		DisableInput(Cast<APlayerController>(GetController()));
	}
	else
	{
		EnableInput(Cast<APlayerController>(GetController()));
	}
}

