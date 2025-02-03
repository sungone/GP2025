// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPPlayerController.h"
#include "Blueprint/UserWidget.h"

AGPPlayerController::AGPPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/UI/WBP_PlayerMainWidget.WBP_PlayerMainWidget_C"));
	if (WidgetBPClass.Succeeded())
	{
		PlayerMainWidgetClass = WidgetBPClass.Class;
	}
}

void AGPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	if (PlayerMainWidgetClass)
	{
		PlayerMainWidget = CreateWidget<UUserWidget>(this, PlayerMainWidgetClass);
		if (PlayerMainWidget)
		{
			PlayerMainWidget->AddToViewport();
		}
	}
}
