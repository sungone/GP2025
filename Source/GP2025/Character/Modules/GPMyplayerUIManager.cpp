// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerUIManager.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/GPInventory.h"
#include "Player/GPPlayerController.h"
#include "UI/GPLobbyWidget.h"
#include "UI/GPInGameWidget.h"
#include "Character/GPCharacterMyplayer.h"
#include "GPMyplayerUIManager.h"
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
}

void UGPMyplayerUIManager::ToggleInventory()
{
	if (!Owner || bInventoryToggled) return;

	bInventoryToggled = true;

	if (InventoryWidget)
	{
		if (InventoryWidget->IsInViewport())
			CloseInventory();
		else
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
	if (!Owner || !InventoryWidget) return;

	if (!InventoryWidget->IsInViewport())
	{
		if (UGPInventory* Inventory = Cast<UGPInventory>(InventoryWidget))
		{
			Inventory->SetGold(Owner->CharacterInfo.Gold);
		}

		InventoryWidget->AddToViewport();

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
	if (!Owner || !InventoryWidget) return;

	if (InventoryWidget->IsInViewport())
	{
		InventoryWidget->RemoveFromParent();

		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly());
		}
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