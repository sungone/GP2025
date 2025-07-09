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
	/** ��ų �̸� ǥ�ÿ� */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillNameText;

	/** ��Ÿ�� ǥ�ÿ� */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooltimeText;

	/** ��ų �� ǥ�ÿ� */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillValueText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillInputKey;

	/** Skill Info ������ ���� �Լ� */
	UFUNCTION(BlueprintCallable)
	void SetSkillInfo(const FName& SkillName, float Cooltime, float SkillValue, const FString& InputKey);
};
