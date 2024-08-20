// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/THHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/THCharacterWidgetInterface.h"

UTHHpBarWidget::UTHHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.f;
}

void UTHHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar);

	ITHCharacterWidgetInterface* CharacterWidget = Cast<ITHCharacterWidgetInterface>(OwningActor);

	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UTHHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	ensure(MaxHp > 0.f);

	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}
