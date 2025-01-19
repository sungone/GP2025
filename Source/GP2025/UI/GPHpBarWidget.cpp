// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/GPCharacterWidgetInterface.h"

UGPHpBarWidget::UGPHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.f;
}

void UGPHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	ensure(MaxHp > 0.f);
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}

void UGPHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName("PbHpBar"));
	ensure(HpProgressBar);

	IGPCharacterWidgetInterface* CharacterWidget = Cast<IGPCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}
