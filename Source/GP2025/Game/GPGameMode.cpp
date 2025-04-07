// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/GPGameMode.h"
#include "Player/GPPlayerController.h"
#include "Network/GPNetworkManager.h"

AGPGameMode::AGPGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default Pawn Class 설정
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/Blueprint/BP_MyPlayer"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	// Player Controller 설정
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/GP2025.GPPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}