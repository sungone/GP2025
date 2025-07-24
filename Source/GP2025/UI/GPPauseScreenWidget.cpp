// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPPauseScreenWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UGPPauseScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddDynamic(this, &UGPPauseScreenWidget::OnQuitGameClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UGPPauseScreenWidget::OnBackClicked);
	}
}

void UGPPauseScreenWidget::OnQuitGameClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, true);
	}
}

void UGPPauseScreenWidget::OnBackClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}


