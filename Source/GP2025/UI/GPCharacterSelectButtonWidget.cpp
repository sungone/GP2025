// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPCharacterSelectButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UGPCharacterSelectButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UGPCharacterSelectButtonWidget::HandleClicked);
	}
}

void UGPCharacterSelectButtonWidget::HandleClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}

	OnCharacterSelected.Broadcast(CharacterType);
}
