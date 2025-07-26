// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPDeadScreenWidget.h"
#include "Character/GPCharacterMyplayer.h"
#include "Kismet/GameplayStatics.h"
#include "Network/GPNetworkManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"    


void UGPDeadScreenWidget::UpdateRespawnMessage(int32 SecondsLeft)
{
    if (!RespawnCount) return;

    FString Message = FString::Printf(TEXT("%d초 뒤에 부활합니다"), SecondsLeft);
    RespawnCount->SetText(FText::FromString(Message));
}

void UGPDeadScreenWidget::StartRespawnCountdown(int32 StartSeconds)
{
    SecondsRemaining = StartSeconds;

    RespawnCount->SetVisibility(ESlateVisibility::Visible);
    UpdateRespawnMessage(SecondsRemaining);
    GetWorld()->GetTimerManager().SetTimer(
        CountdownTimerHandle,
        this,
        &UGPDeadScreenWidget::TickCountdown,
        1.0f,
        true
    );
}
void UGPDeadScreenWidget::TickCountdown()
{
    SecondsRemaining--;

    if (SecondsRemaining > 0)
    {
        UpdateRespawnMessage(SecondsRemaining);
        if (SecondsRemaining == 1)
            PlayFadeIn();
    }
    else if (SecondsRemaining == 0)
    {
        RespawnCount->SetVisibility(ESlateVisibility::Hidden);
    }
    else if (SecondsRemaining < -2)
    {
        GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
        RemoveFromParent(); 
    }
}

void UGPDeadScreenWidget::PlayFadeIn()
{
    if (!FadeOverlay || !FadeInAnim) return;

    
    FadeOverlay->SetVisibility(ESlateVisibility::Visible);

    const float PlayRate = 1.0f;
    PlayAnimation(FadeInAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, PlayRate);
}
