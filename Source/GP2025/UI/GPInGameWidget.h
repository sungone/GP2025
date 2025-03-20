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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float HpPercent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float ExpPercent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FString LevelText;

//public :
//public:
//    virtual void NativeConstruct() override;
//
//    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
//    class UProgressBar* HealthBar;
//
//    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
//    class UProgressBar* ExpBar;
//
//    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
//    class UTextBlock* PlayerLevelText;  
//
//    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
//    class UProgressBar* QSkillBar;
//
//    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
//    class UProgressBar* ESkillBar;
//
//    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
//    class UProgressBar* RSkillBar;
//
//    UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "UI")
//    class UTextBlock* CurrentLocation;
//	
//
//public :
//    void UpdateHealthBar(float Ratio);
//    void UpdateExpBar(float Ratio);
//    void UpdatePlayerLevel(int32 NewLevel);
//
//
//public :
//    class AGPCharacterMyplayer* MyPlayer;
};
