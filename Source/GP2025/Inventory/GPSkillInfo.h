// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPSkillInfo.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class GP2025_API UGPSkillInfo : public UUserWidget
{
	GENERATED_BODY()
	
public :
	/** 스킬 이름 표시용 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillNameText;

	/** 쿨타임 표시용 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooltimeText;

	/** 스킬 값 표시용 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillValueText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillInputKey;

	/** Skill Info 데이터 세팅 함수 */
	UFUNCTION(BlueprintCallable)
	void SetSkillInfo(const FName& SkillName, float Cooltime, float SkillValue, const FString& InputKey);
};
