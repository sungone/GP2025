// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPInGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public :
	virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UTextBlock* LevelText;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UProgressBar* ExpBar;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UProgressBar* QSkillBar;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UProgressBar* ESkillBar;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UProgressBar* RSkillBar;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UTextBlock* QSkillText;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UTextBlock* ESkillText;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
    class UTextBlock* RSkillText;

    UPROPERTY(meta = (BindWidget))
    class UGPQuestListWidget* QuestListWidget;

    UPROPERTY(meta = (BindWidget))
    class UGPChatBoxWidget* ChatBoxWidget;



public :
    void UpdateHealthBar(float Ratio);
    void UpdateExpBar(float Ratio);
    void UpdatePlayerLevel(int32_t NewLevel);

    struct FSlateColor GetQSkillTextColor();
    struct FSlateColor GetESkillTextColor();
    struct FSlateColor GetRSkillTextColor();

public:
    UPROPERTY(meta = (BindWidget))
    class UBorder* GameMessageBox;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* GameMessage;

    void ShowGameMessage(const FString& Message, float Duration = 3.0f);

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Message")
    void PlayGameMessageFadeIn();

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Message")
    void PlayGameMessageFadeOut();

    //FTimerHandle GameMessageTimerHandle;
    //void HideGameMessage();
};
