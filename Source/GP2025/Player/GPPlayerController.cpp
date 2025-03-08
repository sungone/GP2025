// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPPlayerController.h"
#include "Inventory/GPInventory.h"
#include "Blueprint/UserWidget.h"


void AGPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
