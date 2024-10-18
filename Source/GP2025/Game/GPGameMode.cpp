// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/GPGameMode.h"
#include "Player/GPPlayerController.h"
#include "Network/GPGameInstance.h"
#include "Engine/World.h"

AGPGameMode::AGPGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default Pawn Class Setting
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/GP2025.GPCharacterPlayer"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	// PlayerController Setting
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/GP2025.GPPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}

void AGPGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UGPGameInstance* GameInst = Cast<UGPGameInstance>(GWorld->GetGameInstance());
	if (!GameInst)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance cast failed"));
	}

	GameInst->ProcessPacket();
}

