// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPCharacterStatusWidget.h"
#include "Character/GPCharacterBase.h"
#include "UI/GPHpBarWidget.h"
#include "UI/GPLevelWidget.h"
#include "UI/GPUserNameWidget.h"

void UGPCharacterStatusWidget::BindToCharacter(AGPCharacterBase* Character)
{
	if (WBPHpBar)
	{
		WBPHpBar->BindToCharacter(Character);  
	}
	if (WBPLevelText)
	{
		WBPLevelText->BindToCharacter(Character);
	}
	if (WBPName)
	{
		WBPName->BindToCharacter(Character);
	}
}
