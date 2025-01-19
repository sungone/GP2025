// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPWidgetComponent.h"
#include "GPUserWidget.h"

void UGPWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UGPUserWidget* GPUserWidget = Cast<UGPUserWidget>(GetWidget());
	if (GPUserWidget)
	{
		GPUserWidget->SetOwningActor(GetOwner());
	}
}
