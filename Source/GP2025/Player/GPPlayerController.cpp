// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPPlayerController.h"
#include "Inventory/GPInventory.h"
#include "Blueprint/UserWidget.h"
#include "UI/GPInGameWidget.h"
#include "UI/GPLoginWidget.h"
#include "UI/GPLobbyWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Network/GPNetworkManager.h"
#include "GPPlayerController.h"

AGPPlayerController::AGPPlayerController()
{

}

void AGPPlayerController::BeginPlay()
{
	Super::BeginPlay();
}
