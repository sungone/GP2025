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
				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeGameOnly());
				InGameWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
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
			LocalInGameWidget->ShowGameMessage(TEXT("몬스터에게 사망하셨습니다"), 2.f);
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

