// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPPlayerController.h"
#include "Inventory/GPInventory.h"
#include "Blueprint/UserWidget.h"
#include "UI/GPInGameWidget.h"
#include "UI/GPLoginWidget.h"
#include "UI/GPLobbyWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Network/GPNetworkManager.h"

void AGPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

void AGPPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UGPNetworkManager* NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		NetworkMgr->SetMyPlayer();
	}
}

AGPPlayerController::AGPPlayerController()
{
	static ConstructorHelpers::FClassFinder<UGPInGameWidget> InGameWidgetBP(TEXT("/Game/Inventory/Widgets/WBP_InGame"));
	if (InGameWidgetBP.Succeeded())
	{
		InGameWidgetClass = InGameWidgetBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UGPInGameWidget> LobbyWidgetBP(TEXT("/Game/Inventory/Widgets/WBP_Lobby"));
	if (LobbyWidgetBP.Succeeded())
	{
		LobbyWidgetClass = LobbyWidgetBP.Class;
	}
}

void AGPPlayerController::ShowInGameUI()
{
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

void AGPPlayerController::ShowLobbyUI()
{
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
