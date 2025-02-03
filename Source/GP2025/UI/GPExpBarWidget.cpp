// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPExpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/GPCharacterWidgetInterface.h"

UGPExpBarWidget::UGPExpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxExp = -1.0f;
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

void UGPExpBarWidget::UpdateExpBar(float NewCurrentExp)
{
	ensure(MaxExp > 0.f);
	if (ExpProgressBar)
	{
		ExpProgressBar->SetPercent(NewCurrentExp / MaxExp);
	}
}
