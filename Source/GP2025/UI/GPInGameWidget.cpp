// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPInGameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGPInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGPInGameWidget::UpdateHealthBar(float Ratio)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(Ratio);
    }
}

void UGPInGameWidget::UpdateExpBar(float Ratio)
{
    if (ExpBar)
    {
        ExpBar->SetPercent(Ratio);
    }
}

void UGPInGameWidget::UpdatePlayerLevel(__int32 NewLevel)
{
    if (LevelText)
    {
        LevelText->SetText(FText::AsNumber(NewLevel));
    }
}
