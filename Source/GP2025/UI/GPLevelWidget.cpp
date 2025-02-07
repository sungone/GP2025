// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPLevelWidget.h"
#include "Components/TextBlock.h"

void UGPLevelWidget::UpdateLevelText(int32 Level)
{
	if (TextLevel)
	{
		TextLevel->SetText(FText::Format(FText::FromString("{0}"), Level));
	}
}

void UGPLevelWidget::NativeConstruct()
{
	Super::NativeConstruct();


}