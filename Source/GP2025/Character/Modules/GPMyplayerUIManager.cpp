// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerUIManager.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/GPInventory.h"
#include "Player/GPPlayerController.h"
#include "UI/GPInGameWidget.h"
#include "Character/GPCharacterMyplayer.h"

UGPMyplayerUIManager::UGPMyplayerUIManager()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/Inventory/Widgets/WBP_Inventory"));
	if (WidgetBPClass.Succeeded())
	{
		InventoryWidgetClass = WidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> SettingWidgetBPClass(TEXT("/Game/UI/WBP_PauseScreen"));
	if (SettingWidgetBPClass.Succeeded())
	{
		SettingWidgetClass = SettingWidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameWidgetBPClass(TEXT("/Game/UI/WBP_InGame"));
	if (InGameWidgetBPClass.Succeeded())
	{
		InGameWidgetClass = InGameWidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> GunCrosshairWidgetBPClass(TEXT("/Game/UI/WBP_GunCrosshair"));
	if (GunCrosshairWidgetBPClass.Succeeded())
	{
		GunCrosshairWidgetClass = GunCrosshairWidgetBPClass.Class;
	}
}
void UGPMyplayerUIManager::Initialize(AGPCharacterMyplayer* InOwner)
{
	Owner = InOwner;
}

void UGPMyplayerUIManager::OnLoginCreateWidget()
{
	if (!Owner) return;
	UWorld* World = Owner->GetWorld();
	if (!World) return;

	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UUserWidget>(World, InventoryWidgetClass);
	}

	if (SettingWidgetClass)
	{
		SettingWidget = CreateWidget<UUserWidget>(World, SettingWidgetClass);
	}

	if (InGameWidgetClass)
	{
		InGameWidget = CreateWidget<UUserWidget>(World, InGameWidgetClass);
		if (InGameWidget)
		{
			InGameWidget->AddToViewport();
			APlayerController* PC = Cast<APlayerController>(Owner->GetController());
			if (PC)
			{
				PC->SetShowMouseCursor(false);
				PC->SetInputMode(FInputModeGameOnly());
				InGameWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UGPMyplayerUIManager::ToggleInventory()
{
	if (!Owner || bInventoryToggled) return;

	bInventoryToggled = true;

	if (InventoryWidget)
	{
		if (InventoryWidget->IsInViewport())
			CloseInventory();
		else
			OpenInventory();
	}
}

void UGPMyplayerUIManager::ResetToggleInventory()
{
	if (!Owner || !bInventoryToggled) return;
	bInventoryToggled = false;
}

void UGPMyplayerUIManager::OpenInventory()
{
	if (!Owner || !InventoryWidget) return;

	if (!InventoryWidget->IsInViewport())
	{
		InventoryWidget->AddToViewport();

		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}

		UGPInventory* CastInventory = Cast<UGPInventory>(InventoryWidget);
		if (CastInventory)
		{
			CastInventory->SetGold(Owner->CharacterInfo.Gold);
		}
	}

	if (InGameWidget && !InGameWidget->IsInViewport())
	{
		InGameWidget->AddToViewport();
	}
}

void UGPMyplayerUIManager::CloseInventory()
{
	if (!Owner || !InventoryWidget) return;

	if (InventoryWidget->IsInViewport())
	{
		InventoryWidget->RemoveFromParent();

		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
}

void UGPMyplayerUIManager::OpenSettingWidget()
{
	if (!Owner || !SettingWidget) return;

	if (!SettingWidget->IsInViewport())
	{
		SettingWidget->AddToViewport();

		APlayerController* PC = Cast<APlayerController>(Owner->GetController());
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}

void UGPMyplayerUIManager::ShowInGameUI()
{
	if (InGameWidget && !InGameWidget->IsInViewport())
	{
		InGameWidget->AddToViewport();
	}
}

UGPInGameWidget* UGPMyplayerUIManager::GetInGameWidget()
{
	UGPInGameWidget* Inven = Cast<UGPInGameWidget>(InGameWidget);
	if (Inven)
		return Inven;
	else
		return nullptr;
}

UGPInventory* UGPMyplayerUIManager::GetInventoryWidget()
{
	UGPInventory* Inven = Cast<UGPInventory>(InventoryWidget);
	if (Inven)
		return Inven;
	else
		return nullptr;
}