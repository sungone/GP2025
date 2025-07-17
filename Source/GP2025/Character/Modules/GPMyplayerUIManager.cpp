// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerUIManager.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/GPInventory.h"
#include "Player/GPPlayerController.h"
#include "UI/GPLobbyWidget.h"
#include "UI/GPInGameWidget.h"
#include "Character/GPCharacterMyplayer.h"
#include "UI/GPChatBoxWidget.h"
#include "GPMyplayerUIManager.h"
#include "UI/GPDeadScreenWidget.h"
#include "UI/GPQuestListWidget.h"
#include "Character/Modules/GPMyplayerSoundManager.h"
#include "UI/GPSkillLevelUpText.h"
#include "Skill/GPSkillStruct.h"
#include "Components/Button.h"
#include "UI/Friend/GPFriendBox.h"
#include "Quest/GPQuestMessageStruct.h"
#include "UI/GPQuestWidget.h"
#include "UI/Friend/GPFriendBox.h"
#include "Inventory/GPSkillInfo.h"

UGPMyplayerUIManager::UGPMyplayerUIManager()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/Inventory/Widgets/WBP_Inventory"));
	if (WidgetBPClass.Succeeded())
	{
		InventoryWidgetClass = WidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> SettingWidgetBPClass(TEXT("/Game/UI/WBP_PauseScreen"));
	if (SettingWidgetBPClass.Succeeded())
	{
		SettingWidgetClass = SettingWidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameWidgetBPClass(TEXT("/Game/UI/WBP_InGame"));
	if (InGameWidgetBPClass.Succeeded())
	{
		InGameWidgetClass = InGameWidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> GunCrosshairWidgetBPClass(TEXT("/Game/UI/WBP_GunCrosshair"));
	if (GunCrosshairWidgetBPClass.Succeeded())
	{
		GunCrosshairWidgetClass = GunCrosshairWidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyWidgetClassBPClass(TEXT("/Game/UI/WBP_Lobby"));
	if (LobbyWidgetClassBPClass.Succeeded())
	{
		LobbyWidgetClass = LobbyWidgetClassBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> LoginWidgetClassBPClass(TEXT("/Game/UI/WBP_Login"));
	if (LoginWidgetClassBPClass.Succeeded())
	{
		LoginWidgetClass = LoginWidgetClassBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> DeadWidgetClassBPClass(TEXT("/Game/UI/WBP_DeadScreen"));
	if (DeadWidgetClassBPClass.Succeeded())
	{
		DeadScreenWidgetClass = DeadWidgetClassBPClass.Class;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> QuestTableObj(TEXT("/Game/Quest/GPQuestMessageTable.GPQuestMessageTable"));
	if (QuestTableObj.Succeeded())
	{
		QuestMessageTable = QuestTableObj.Object;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> FriendBoxBPClass(TEXT("/Game/UI/Friend/WBP_FriendBox"));
	if (FriendBoxBPClass.Succeeded())
	{
		FriendBoxWidgetClass = FriendBoxBPClass.Class;
	}
}
void UGPMyplayerUIManager::Initialize(AGPCharacterMyplayer* InOwner)
{
	Owner = InOwner;
}

void UGPMyplayerUIManager::OnSetUpInGameWidgets()
{
	if (!Owner) return;
	UWorld* World = Owner->GetWorld();
	if (!World) return;

	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UUserWidget>(World, InventoryWidgetClass);
	}

	if (SettingWidgetClass)
	{
		SettingWidget = CreateWidget<UUserWidget>(World, SettingWidgetClass);
	}

	if (DeadScreenWidgetClass)
	{
		DeadScreenWidget = CreateWidget<UUserWidget>(World, DeadScreenWidgetClass);
	}

	if (GunCrosshairWidgetClass && Owner->bIsGunnerCharacter())
	{
		GunCrosshairWidget = CreateWidget<UUserWidget>(World, GunCrosshairWidgetClass);

		if (GunCrosshairWidget)
		{
			GunCrosshairWidget->AddToViewport();
			GunCrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (InGameWidgetClass)
	{
		InGameWidget = CreateWidget<UUserWidget>(World, InGameWidgetClass);
		if (InGameWidget)
		{
			InGameWidget->AddToViewport();
			APlayerController* PC = Cast<APlayerController>(Owner->GetController());
			if (PC)
			{
				PC->SetShowMouseCursor(false);
				PC->SetInputMode(FInputModeGameOnly());
				InGameWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}

	if (FriendBoxWidgetClass && !FriendBoxWidget)
	{
		FriendBoxWidget = CreateWidget<UGPFriendBox>(World, FriendBoxWidgetClass);
		FriendBoxWidget->AddToViewport();
		FriendBoxWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGPMyplayerUIManager::ToggleInventory()
{
	if (!Owner || bInventoryToggled || !InventoryWidget || !IsValid(InventoryWidget)) return;

	bInventoryToggled = true;

	if (InventoryWidget->IsInViewport())
	{
		CloseInventory();
	}
	else
	{
		OpenInventory();
	}
}

void UGPMyplayerUIManager::ResetToggleInventory()
{
	if (!Owner || !bInventoryToggled) return;
	bInventoryToggled = false;
}

void UGPMyplayerUIManager::OpenInventory()
{
	if (!Owner || !IsValid(InventoryWidget)) return;

	if (!InventoryWidget->IsInViewport())
	{
		UGPInventory* Inventory = Cast<UGPInventory>(InventoryWidget);
		if (Inventory && IsValid(Owner))
		{
			Inventory->SetGold(Owner->CharacterInfo.Gold);
			UpdateSkillInfosFromPlayer();
		}

		InventoryWidget->AddToViewport();
		Inventory->OpenInventory();
		bIsInventoryOpen = true;

		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}

	if (InGameWidget && !InGameWidget->IsInViewport())
	{
		InGameWidget->AddToViewport();
	}
}

void UGPMyplayerUIManager::CloseInventory()
{
	if (!Owner || !IsValid(InventoryWidget))
		return;

	UGPInventory* Inventory = Cast<UGPInventory>(InventoryWidget);
	if (InventoryWidget->IsInViewport())
	{
		// 애니메이션 먼저 실행
		if (Inventory)
		{
			Inventory->CloseInventory();
		}

		FTimerHandle TimerHandle;
		Owner->GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this]()
				{
					if (IsValid(InventoryWidget))
					{
						InventoryWidget->RemoveFromParent();
					}

					bIsInventoryOpen = false;

					APlayerController* PC = Cast<APlayerController>(Owner->GetController());
					if (PC)
					{
						PC->SetShowMouseCursor(false);
						PC->SetInputMode(FInputModeGameOnly());
					}
				}),
			0.3f,
			false
		);
	}
}

void UGPMyplayerUIManager::OpenSettingWidget()
{
	if (!Owner || !SettingWidget) return;

	if (!SettingWidget->IsInViewport())
	{
		SettingWidget->AddToViewport();

		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}

void UGPMyplayerUIManager::ShowInGameUI()
{
	if (InGameWidget && !InGameWidget->IsInViewport())
	{
		InGameWidget->AddToViewport();
	}
}

void UGPMyplayerUIManager::ShowDeadScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowDeadScreen() called")); // ① 함수 진입 확인

	if (!DeadScreenWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("DeadScreenWidgetClass is NULL"));
		return;
	}

	if (!DeadScreenWidget)
	{
		DeadScreenWidget = CreateWidget<UGPDeadScreenWidget>(GetWorld(), DeadScreenWidgetClass);
	}

	UGPDeadScreenWidget* TypedWidget = Cast<UGPDeadScreenWidget>(DeadScreenWidget);
	if (TypedWidget)
	{
		if (UGPInGameWidget* LocalInGameWidget = Cast<UGPInGameWidget>(InGameWidget))
		{
			LocalInGameWidget->ShowGameMessage(FText::FromString(TEXT("몬스터에게 사망하셨습니다")), 2.f);
		}

		TypedWidget->AddToViewport();
		TypedWidget->PlayFadeOut();
		TypedWidget->StartRespawnCountdown(3);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast DeadScreenWidget to UGPDeadScreenWidget"));
	}
}

void UGPMyplayerUIManager::AddQuestEntry(uint8 QuestType, bool bIsSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("=== [UIManager] AddQuestEntry called: QuestType = %d, bIsSuccess = %s ==="), QuestType, bIsSuccess ? TEXT("true") : TEXT("false"));

	UGPQuestListWidget* QuestList = GetQuestListWidget();
	if (QuestList)
	{
		UE_LOG(LogTemp, Warning, TEXT("=== [UIManager] QuestListWidget is valid. Calling AddQuestEntry to QuestListWidget ==="));
		QuestList->AddQuestEntry(QuestType, bIsSuccess);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UIManager] QuestListWidget is NULL"));
	}
}

void UGPMyplayerUIManager::UpdateQuestState(uint8 QuestType, bool bIsSuccess)
{
	UGPQuestListWidget* QuestList = GetQuestListWidget();
	if (QuestList)
	{
		QuestList->UpdateQuestState(QuestType, bIsSuccess);
	}
}


UGPQuestListWidget* UGPMyplayerUIManager::GetQuestListWidget()
{
	UGPInGameWidget* InGame = GetInGameWidget();
	if (InGame)
	{
		return InGame->QuestListWidget;
	}
	return nullptr;
}

UGPInGameWidget* UGPMyplayerUIManager::GetInGameWidget()
{
	UGPInGameWidget* Inven = Cast<UGPInGameWidget>(InGameWidget);
	if (Inven)
		return Inven;
	else
		return nullptr;
}

UGPInventory* UGPMyplayerUIManager::GetInventoryWidget()
{
	UGPInventory* Inven = Cast<UGPInventory>(InventoryWidget);
	if (Inven)
		return Inven;
	else
		return nullptr;
}

UGPChatBoxWidget* UGPMyplayerUIManager::GetChatBoxWidget()
{
	return Cast<UGPChatBoxWidget>(GetInGameWidget()->ChatBoxWidget);
}

void UGPMyplayerUIManager::ShowLobbyUI()
{
	if (!Owner) return;
	UWorld* World = Owner->GetWorld();
	if (!World) return;

	if (!LobbyWidget && LobbyWidgetClass)
	{
		LobbyWidget = CreateWidget<UUserWidget>(World, LobbyWidgetClass);
	}

	if (LobbyWidget && !LobbyWidget->IsInViewport())
	{
		LobbyWidget->AddToViewport();
	}

	if (Owner && Owner->IsPlayerControlled())
	{
		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->bShowMouseCursor = true;

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(LobbyWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
		}
	}
}

void UGPMyplayerUIManager::ShowLoginUI()
{
	if (!Owner) return;
	UWorld* World = Owner->GetWorld();
	if (!World) return;

	// 위젯이 아직 생성되지 않았다면 생성
	if (!LoginWidget && LoginWidgetClass)
	{
		LoginWidget = CreateWidget<UUserWidget>(World, LoginWidgetClass);
	}

	// 이미 Viewport에 없으면 추가
	if (LoginWidget && !LoginWidget->IsInViewport())
	{
		LoginWidget->AddToViewport();
		if (Owner->SoundManager)
		{
			Owner->SoundManager->PlayLoginBGM(); // Login Sound
		}
	}

	// 마우스 커서와 입력 모드 설정
	if (Owner && Owner->IsPlayerControlled())
	{
		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->bShowMouseCursor = true;

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(LoginWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
		}
	}
}

void UGPMyplayerUIManager::SpawnSkillLevelText(int32 NewLevel)
{
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("[SkillText] Spawn failed: Owner is null"));
		return;
	}

	UWorld* World = Owner->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[SkillText] Spawn failed: World is null"));
		return;
	}

	FVector SpawnLocation = Owner->GetActorLocation() + FVector(0, 0, 0);
	UE_LOG(LogTemp, Log, TEXT("[SkillText] Spawn location: %s"), *SpawnLocation.ToString());

	// 텍스트 클래스 로딩
	TSubclassOf<AGPSkillLevelUpText> TextClass = LoadClass<AGPSkillLevelUpText>(nullptr, TEXT("/Game/Blueprint/BP_SkillLevelUpText.BP_SkillLevelUpText_C"));
	if (!TextClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[SkillText] Spawn failed: Could not load BP_SkillLevelUpText class"));
		return;
	}

	// 액터 생성
	AGPSkillLevelUpText* TextActor = World->SpawnActor<AGPSkillLevelUpText>(TextClass, SpawnLocation, FRotator::ZeroRotator);
	if (!TextActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[SkillText] Spawn failed: Could not spawn text actor"));
		return;
	}

	// 텍스트 설정
	UE_LOG(LogTemp, Log, TEXT("[SkillText] Actor spawned successfully. Setting level text for level %d"), NewLevel);
	TextActor->SetSkillLevelUpText(NewLevel);
}

void UGPMyplayerUIManager::FocusChatInput()
{
	UGPInGameWidget* InGame = GetInGameWidget();
	if (!InGame || !Owner) return;

	if (UGPChatBoxWidget* Chat = InGame->ChatBoxWidget)
	{
		Chat->SetVisibility(ESlateVisibility::Visible);
		Chat->SetKeyboardFocusToInput();

		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(true);

			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(Chat->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
		}
	}
}

void UGPMyplayerUIManager::UpdateSkillInfosFromPlayer()
{
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("[UIManager] UpdateSkillInfosFromPlayer: Owner is null"));
		return;
	}

	UGPInventory* Inven = GetInventoryWidget();
	if (!Inven)
	{
		UE_LOG(LogTemp, Error, TEXT("[UIManager] UpdateSkillInfosFromPlayer: Inventory is null"));
		return;
	}

	if (!Inven->SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[UIManager] UpdateSkillInfosFromPlayer: SkillDataTable is null"));
		return;
	}

	bool bIsGunner = Owner->bIsGunnerCharacter();
	FString RequiredWeapon = bIsGunner ? TEXT("Gun") : TEXT("Sword");

	int32 PlayerLevel = Owner->CharacterInfo.GetLevel();

	TArray<FString> SkillNames;
	TArray<FString> SkillKeys;

	if (bIsGunner)
	{
		SkillNames = { TEXT("던지기"), TEXT("빨리던지기"), TEXT("분노") };
		SkillKeys = { TEXT(" Q "), TEXT(" E "), TEXT(" R ") };
	}
	else
	{
		SkillNames = { TEXT("강타"), TEXT("돌진"), TEXT("회오리베기") };
		SkillKeys = { TEXT(" Q "), TEXT(" E "), TEXT(" R ") };
	}

	TArray<UGPSkillInfo*> SkillWidgets = {
	Inven->QSkillInfo,
	Inven->ESkillInfo,
	Inven->RSkillInfo
	};

	TArray<bool> bSkillVisible = { false, false, false };

	if (PlayerLevel >= 4)
	{
		bSkillVisible[0] = true;
		bSkillVisible[1] = true;
		bSkillVisible[2] = true;
	}
	else if (PlayerLevel == 3)
	{
		bSkillVisible[0] = true;
		bSkillVisible[1] = true;
	}
	else if (PlayerLevel == 2)
	{
		bSkillVisible[0] = true;
	}



	for (int32 i = 0; i < SkillNames.Num() && i < SkillWidgets.Num(); ++i)
	{
		if (!SkillWidgets[i])
			continue;

		// 먼저 Visibility 설정
		if (bSkillVisible[i])
		{
			SkillWidgets[i]->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			SkillWidgets[i]->SetVisibility(ESlateVisibility::Hidden);
			continue;
		}

		const FString& SkillName = SkillNames[i];
		const FString& InputKey = SkillKeys[i];

		const FGPSkillStruct* BestRow = nullptr;
		int32 BestSkillLv = -1;
		int32 BestAppliedLevel = -1;

		for (const auto& RowPair : Inven->SkillDataTable->GetRowMap())
		{
			const FGPSkillStruct* Row = reinterpret_cast<const FGPSkillStruct*>(RowPair.Value);
			if (!Row)
				continue;

			if (Row->weapon != RequiredWeapon)
				continue;

			if (Row->name != SkillName)
				continue;

			if (Row->applied_Level > PlayerLevel)
				continue;

			// Applied Level 가 더 높거나 같으면 교체
			if (Row->applied_Level > BestAppliedLevel ||
				(Row->applied_Level == BestAppliedLevel && Row->skill_lv > BestSkillLv))
			{
				BestAppliedLevel = Row->applied_Level;
				BestSkillLv = Row->skill_lv;
				BestRow = Row;
			}
		}

		if (BestRow && SkillWidgets[i])
		{
			SkillWidgets[i]->SetSkillInfo(
				FName(*BestRow->name),
				BestRow->cooltime,
				BestRow->skill_value_00,
				InputKey
			);
		}
		else if (SkillWidgets[i])
		{
			// 값 없으면 초기화
			SkillWidgets[i]->SetSkillInfo(
				FName(TEXT("-")),
				0.0f,
				0,
				InputKey
			);
		}
	}
}

void UGPMyplayerUIManager::ShowQuestStartMessage(QuestType InQuestType)
{
	if (QuestMessageTable)
	{
		FName RowName = *FString::Printf(TEXT("%d"), static_cast<uint8>(InQuestType));
		FGPQuestMessageStruct* Row = QuestMessageTable->FindRow<FGPQuestMessageStruct>(
			RowName,
			TEXT("Quest Message Lookup")
		);

		if (Row->QuestID == 100) // 튜토리얼 시작 메세지를 Return
			return;

		if (GetInGameWidget())
		{
			GetInGameWidget()->ShowGameMessage(Row->QuestMessage, 3.0f);
		}
	}
}

void UGPMyplayerUIManager::PlayTutorialQuestWidget()
{
	if (!Owner) return;

	UWorld* World = Owner->GetWorld();
	if (!World) return;

	// 위젯 클래스 로드
	TSubclassOf<UGPQuestWidget> WidgetClass = LoadClass<UGPQuestWidget>(
		nullptr,
		TEXT("/Game/UI/WBP_TutorialQuest.WBP_TutorialQuest_C")
	);
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[UIManager] Failed to load TutorialQuest widget class."));
		return;
	}

	// 기존 퀘스트 위젯 제거
	if (CurrentQuestWidget)
	{
		CurrentQuestWidget->RemoveFromParent();
		CurrentQuestWidget = nullptr;
	}

	// 위젯 생성 및 표시
	UGPQuestWidget* TutorialWidget = CreateWidget<UGPQuestWidget>(World, WidgetClass);
	if (TutorialWidget)
	{
		TutorialWidget->AddToViewport();
		CurrentQuestWidget = TutorialWidget;

		UE_LOG(LogTemp, Log, TEXT("[UIManager] Tutorial quest widget displayed."));

		if (APlayerController* PC = Cast<APlayerController>(Owner->GetController()))
		{
			PC->bShowMouseCursor = true;

			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetWidgetToFocus(TutorialWidget->TakeWidget());

			PC->SetInputMode(InputMode);
		}
	}
}

void UGPMyplayerUIManager::OpenFriendBox()
{
	if (!Owner || !FriendBoxWidgetClass) return;

	if (FriendBoxWidget)
	{
		FriendBoxWidget->SetVisibility(ESlateVisibility::Visible);
		FriendBoxWidget->PlayOpenAnimation();

		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(true);

			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(FriendBoxWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
		}

		UE_LOG(LogTemp, Log, TEXT("[UIManager] FriendBoxWidget opened."));
	}
}

void UGPMyplayerUIManager::CloseFriendBox()
{
	if (!Owner || !FriendBoxWidget) return;
	FriendBoxWidget->PlayCloseAnimation();
	FriendBoxWidget->SetVisibility(ESlateVisibility::Hidden);
	APlayerController* PC = Cast<APlayerController>(Owner->GetController());
	if (PC)
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}

	UE_LOG(LogTemp, Log, TEXT("[UIManager] FriendBoxWidget closed."));
}

UGPFriendBox* UGPMyplayerUIManager::GetFriendBoxWidget()
{
	return Cast<UGPFriendBox>(FriendBoxWidget);
}


void UGPMyplayerUIManager::ShowSkillUnlockMessage(ESkillGroup SkillGroup)
{
	if (!GetChatBoxWidget())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UIManager] ChatBoxWidget is null."));
		return;
	}

	FString SkillName;

	switch (SkillGroup)
	{
	case ESkillGroup::HitHard:
		SkillName = TEXT("강타");
		break;
	case ESkillGroup::Clash:
		SkillName = TEXT("돌진");
		break;
	case ESkillGroup::Whirlwind:
		SkillName = TEXT("회오리베기");
		break;
	case ESkillGroup::Throwing:
		SkillName = TEXT("던지기");
		break;
	case ESkillGroup::FThrowing:
		SkillName = TEXT("빨리던지기");
		break;
	case ESkillGroup::Anger:
		SkillName = TEXT("분노");
		break;
	default:
		SkillName = TEXT("");
		break;
	}

	FString Message = FString::Printf(TEXT("스킬이 해금되었습니다."));
	const uint8 SkillChannel = 4;

	GetChatBoxWidget()->AddChatMessage(SkillChannel, SkillName, Message);
}

void UGPMyplayerUIManager::ShowSkillUpgradeMessage(ESkillGroup SkillGroup)
{
	if (!GetChatBoxWidget())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UIManager] ChatBoxWidget is null."));
		return;
	}

	FString SkillName;

	switch (SkillGroup)
	{
	case ESkillGroup::HitHard:
		SkillName = TEXT("강타");
		break;
	case ESkillGroup::Clash:
		SkillName = TEXT("돌진");
		break;
	case ESkillGroup::Whirlwind:
		SkillName = TEXT("회오리베기");
		break;
	case ESkillGroup::Throwing:
		SkillName = TEXT("던지기");
		break;
	case ESkillGroup::FThrowing:
		SkillName = TEXT("빨리던지기");
		break;
	case ESkillGroup::Anger:
		SkillName = TEXT("분노");
		break;
	default:
		SkillName = TEXT("");
		break;
	}

	FString Message = FString::Printf(TEXT("스킬이 업그레이드 되었습니다."));
	const uint8 SkillChannel = 4;

	GetChatBoxWidget()->AddChatMessage(SkillChannel, SkillName, Message);
}

void UGPMyplayerUIManager::AddFriendSystemMessage(EChatFriendNotifyType NotifyType, const FString& NickName)
{
	if (!GetChatBoxWidget()) return;

	const uint8 SystemChannel = 1; // 예: 아이템 채널 또는 시스템 채널
	FString Sender = NickName;
	FString Message;

	switch (NotifyType)
	{
	case EChatFriendNotifyType::RequestReceived:
		Message = TEXT("친구 신청을 보냈습니다.");
		break;
	case EChatFriendNotifyType::RequestSent:
		Message = TEXT("친구 신청을 보냈습니다.");
		break;
	case EChatFriendNotifyType::RequestRejected:
		Message = TEXT("친구 신청을 거절했습니다.");
		break;
	case EChatFriendNotifyType::RequestCancelled:
		Message = TEXT("보낸 친구 신청을 취소했습니다.");
		break;
	case EChatFriendNotifyType::Accepted:
		Message = TEXT("친구가 되었습니다!");
		break;
	case EChatFriendNotifyType::AlreadyFriend:
		Message = TEXT("이미 친구입니다.");
		break;
	case EChatFriendNotifyType::AlreadyRequested:
		Message = TEXT("이미 친구 요청을 보냈습니다.");
		break;
	}

	GetChatBoxWidget()->AddChatMessage(SystemChannel, Sender, Message);
}

void UGPMyplayerUIManager::AddFriendSystemMessage(EChatFriendNotifyType NotifyType)
{
	if (!GetChatBoxWidget()) return;

	const uint8 SystemChannel = 1; // 예: 아이템 채널 또는 시스템 채널
	FString Sender = TEXT("");
	FString Message;

	switch (NotifyType)
	{
	case EChatFriendNotifyType::RequestReceived:
		Message = TEXT("친구 신청을 보냈습니다.");
		break;
	case EChatFriendNotifyType::RequestSent:
		Message = TEXT("친구 신청을 보냈습니다.");
		break;
	case EChatFriendNotifyType::RequestRejected:
		Message = TEXT("친구 신청을 거절했습니다.");
		break;
	case EChatFriendNotifyType::RequestCancelled:
		Message = TEXT("보낸 친구 신청을 취소했습니다.");
		break;
	case EChatFriendNotifyType::Accepted:
		Message = TEXT("친구가 되었습니다!");
		break;
	case EChatFriendNotifyType::AlreadyFriend:
		Message = TEXT("이미 친구입니다.");
		break;
	case EChatFriendNotifyType::AlreadyRequested:
		Message = TEXT("이미 친구 요청을 보냈습니다.");
		break;
	case EChatFriendNotifyType::SelfRequest:
		Sender = TEXT("시스템");
		Message = TEXT("자기 자신에게는 친구 요청을 보낼 수 없습니다.");
		break;
	case EChatFriendNotifyType::NotFound:
		Sender = TEXT("시스템");
		Message = TEXT("해당 유저를 찾을 수 없습니다.");
		break;
	case EChatFriendNotifyType::Removed:
		Message = TEXT("친구 목록에서 제거했습니다.");
		break;
	case EChatFriendNotifyType::DBError:
		Sender = TEXT("시스템");
		Message = TEXT("친구 처리 중 데이터베이스 오류가 발생했습니다.");
		break;
	case EChatFriendNotifyType::UnknownError:
	default:
		Sender = TEXT("시스템");
		Message = TEXT("알 수 없는 오류가 발생했습니다.");
		break;
	}

	GetChatBoxWidget()->AddChatMessage(SystemChannel, Sender, Message);
}

void UGPMyplayerUIManager::AddFriendSystemMessage(int32 Result)
{
	if (!GetChatBoxWidget()) return;

	const uint8 SystemChannel = 1;
	FString Sender = TEXT(""); // 공란 처리
	FString Message;

	if (Result == 0)
	{
		Message = TEXT("친구 요청이 성공적으로 처리되었습니다!");
	}
	else if (Result == -20)
	{
		Message = TEXT("이미 친구 요청을 보낸 대상입니다.");
	}
	else if (Result == -21)
	{
		Message = TEXT("이미 친구로 등록된 대상입니다.");
	}
	else if (Result == -22)
	{
		Message = TEXT("자기 자신에게는 친구 요청을 보낼 수 없습니다.");
	}
	else if (Result == -23)
	{
		Message = TEXT("해당 유저를 찾을 수 없습니다.");
	}
	else if (Result == -99)
	{
		Message = TEXT("친구 요청 처리 중 데이터베이스 오류가 발생했습니다.");
	}
	else
	{
		Message = TEXT("알 수 없는 오류가 발생했습니다.");
	}

	GetChatBoxWidget()->AddChatMessage(SystemChannel, Sender, Message);
}