// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPSkillInfo.h"
#include "Components/TextBlock.h"


void UGPSkillInfo::SetSkillInfo(const FName& SkillName, float Cooltime, float SkillValue, const FString& InputKey)
{
	if (SkillNameText)
	{
		SkillNameText->SetText(FText::FromName(SkillName));
	}

	if (CooltimeText)
	{
		CooltimeText->SetText(FText::FromString(FString::Printf(TEXT("%.1f 초"), Cooltime)));
	}

	if (SkillValueText)
	{
		SkillValueText->SetText(FText::AsNumber(SkillValue));
	}

	if (SkillInputKey)
	{
		SkillInputKey->SetText(FText::FromString(InputKey));
	}
}
