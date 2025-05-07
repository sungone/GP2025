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
    void UpdatePlayerLevel(__int32 NewLevel);

    struct FSlateColor GetQSkillTextColor();
    struct FSlateColor GetESkillTextColor();
    struct FSlateColor GetRSkillTextColor();
};
