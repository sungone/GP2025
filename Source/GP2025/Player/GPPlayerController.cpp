// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPPlayerController.h"

void AGPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
