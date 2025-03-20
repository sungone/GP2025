// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPPlayerController.h"
#include "Inventory/GPInventory.h"
#include "Blueprint/UserWidget.h"
#include "UI/GPInGameWidget.h"
#include "UI/GPLoginWidget.h"
#include "UI/GPLobbyWidget.h"
#include "UObject/ConstructorHelpers.h"


void AGPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

AGPPlayerController::AGPPlayerController()
{
	static ConstructorHelpers::FClassFinder<UGPInGameWidget> InGameWidgetBP(TEXT("/Game/Inventory/Widgets/WBP_InGame"));
	if (InGameWidgetBP.Succeeded())
	{
		InGameWidgetClass = InGameWidgetBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UGPInGameWidget> LoginWidgetBP(TEXT("/Game/Inventory/Widgets/WBP_Login"));
	if (LoginWidgetBP.Succeeded())
	{
		LoginWidgetClass = LoginWidgetBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UGPInGameWidget> LobbyWidgetBP(TEXT("/Game/Inventory/Widgets/WBP_Lobby"));
	if (LobbyWidgetBP.Succeeded())
	{
		LobbyWidgetClass = LobbyWidgetBP.Class;
	}
}

void AGPPlayerController::ShowInGameUI()
{
	if (LoginWidget)
	{
		LoginWidget->RemoveFromParent();  
	}

	if (LobbyWidget)
	{
		LobbyWidget->RemoveFromParent();  
	}

	if (InGameWidgetClass)
	{
		InGameWidget = CreateWidget<UGPInGameWidget>(this, InGameWidgetClass);
		if (InGameWidget)
		{
			InGameWidget->AddToViewport();
			bShowMouseCursor = false;  // 마우스 커서 숨기기
			SetInputMode(FInputModeGameOnly());  // 게임 입력 모드 설정
		}
	}
}

void AGPPlayerController::ShowLoginUI()
{
	if (LobbyWidget)
	{
		LobbyWidget->RemoveFromParent();
	}

	if (LoginWidgetClass)
	{
		LoginWidget = CreateWidget<UGPLoginWidget>(this, LoginWidgetClass);
		if (LoginWidget)
		{
			LoginWidget->AddToViewport();
			bShowMouseCursor = true;  // 마우스 커서 보이기
			SetInputMode(FInputModeUIOnly());  // UI 입력 모드 설정
		}
	}
}

void AGPPlayerController::ShowLobbyUI()
{
	if (LoginWidget)
	{
		LoginWidget->RemoveFromParent(); 
	}

	if (LobbyWidgetClass)
	{
		LobbyWidget = CreateWidget<UGPLobbyWidget>(this, LobbyWidgetClass);
		if (LobbyWidget)
		{
			LobbyWidget->AddToViewport();
			bShowMouseCursor = true; 
			SetInputMode(FInputModeUIOnly()); 
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Show Lobby"));
}
