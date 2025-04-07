// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GPLoginGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "UI/GPLoginWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Network/GPNetworkManager.h"

AGPLoginGameMode::AGPLoginGameMode()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> LoginUIClass(TEXT("/Game/UI/WBP_Login"));
	if (LoginUIClass.Succeeded())
	{
		LoginWidgetClass = LoginUIClass.Class;
	}
}

void AGPLoginGameMode::BeginPlay()
{
	if (LoginWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			UUserWidget* LoginUI = CreateWidget<UUserWidget>(PC, LoginWidgetClass);
			if (LoginUI)
			{
				LoginUI->AddToViewport();
				PC->bShowMouseCursor = true;
				PC->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}
