// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPExpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/GPCharacterWidgetInterface.h"

void UGPExpBarWidget::UpdateExpBar(float ExpRatio)
{
	if (ExpProgressBar)
	{
		ExpProgressBar->SetPercent(ExpRatio);
	}
}

void UGPExpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ExpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbExpBar")));
	ensure(ExpProgressBar);

	IGPCharacterWidgetInterface* CharacterWidget = Cast<IGPCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}