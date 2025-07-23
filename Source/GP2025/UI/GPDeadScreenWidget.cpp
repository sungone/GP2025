// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPDeadScreenWidget.h"
#include "Character/GPCharacterMyplayer.h"
#include "Kismet/GameplayStatics.h"
#include "Network/GPNetworkManager.h"
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
    }
    else if (SecondsRemaining == 0)
    {
        RespawnCount->SetVisibility(ESlateVisibility::Hidden);
        OnRespawnComplete.Broadcast();
    }
    else if (SecondsRemaining < 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
        RemoveFromParent(); 
    }
}
