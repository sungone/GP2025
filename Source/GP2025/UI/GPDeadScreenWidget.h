// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPDeadScreenWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRespawnComplete);

/**
 * 
 */
UCLASS()
class GP2025_API UGPDeadScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
    UFUNCTION(BlueprintImplementableEvent)
    void PlayFadeOut();

    void UpdateRespawnMessage(int32 SecondsLeft);

    void StartRespawnCountdown(int32 StartSeconds);

private:
    FTimerHandle CountdownTimerHandle;
    int32 SecondsRemaining;

    void TickCountdown(); // 내부 호출용

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RespawnCount;

public :
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FadeInAnim;

    void PlayFadeAnim();

    UPROPERTY(meta = (BindWidget))
    class UImage* FadeOverlay;
};
