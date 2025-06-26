// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPInGameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"
#include "GPInGameWidget.h"

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

void UGPInGameWidget::UpdatePlayerLevel(int32_t NewLevel)
{
    if (LevelText)
    {
        LevelText->SetText(FText::AsNumber(NewLevel));
    }
}

FSlateColor UGPInGameWidget::GetQSkillTextColor()
{
    if (QSkillBar && QSkillBar->GetPercent() >= 1.0f - KINDA_SMALL_NUMBER)
    {
        return FSlateColor(FLinearColor::Blue); // 쿨타임 완료: 파란색
    }

    return FSlateColor(FLinearColor::White); // 기본 텍스트 색
}

FSlateColor UGPInGameWidget::GetESkillTextColor()
{
    if (ESkillBar && ESkillBar->GetPercent() >= 1.0f - KINDA_SMALL_NUMBER)
    {
        return FSlateColor(FLinearColor::Blue);
    }

    return FSlateColor(FLinearColor::White);
}

FSlateColor UGPInGameWidget::GetRSkillTextColor()
{
    if (RSkillBar && RSkillBar->GetPercent() >= 1.0f - KINDA_SMALL_NUMBER)
    {
        return FSlateColor(FLinearColor::Blue);
    }

    return FSlateColor(FLinearColor::White);
}

void UGPInGameWidget::ShowGameMessage(const FString& Message, float Duration)
{
    if (!GameMessage) return;

    GameMessage->SetText(FText::FromString(Message));
    GameMessageBox->SetVisibility(ESlateVisibility::Visible);
    // GameMessage->SetVisibility(ESlateVisibility::Visible);

    PlayGameMessageFadeIn();

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            PlayGameMessageFadeOut();
            FTimerHandle HideHandle;
            GetWorld()->GetTimerManager().SetTimer(HideHandle, [this]()
                {
                    GameMessageBox->SetVisibility(ESlateVisibility::Collapsed);
                }, 1.0f, false); 

        }, Duration, false);
}

//void UGPInGameWidget::HideGameMessage()
//{
//    PlayGameMessageFadeOut();
//}


