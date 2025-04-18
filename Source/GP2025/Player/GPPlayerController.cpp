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

}
