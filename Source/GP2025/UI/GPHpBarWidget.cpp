// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Character/GPCharacterBase.h"
#include "Interface/GPCharacterWidgetInterface.h"

UGPHpBarWidget::UGPHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UGPHpBarWidget::UpdateHpBar(float HpRatio)
{
    if (HpProgressBar)
    {
        HpProgressBar->SetPercent(HpRatio);
    }
}

void UGPHpBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
    ensure(HpProgressBar);

    AGPCharacterBase* Character = Cast<AGPCharacterBase>(OwningActor);
    if (Character)
    {
        Character->SetupCharacterWidget(this);
    }
}