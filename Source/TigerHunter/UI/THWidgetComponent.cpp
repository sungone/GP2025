// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/THWidgetComponent.h"
#include "THUserWidget.h"

void UTHWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UTHUserWidget* THUserWidget = Cast<UTHUserWidget>(GetWidget());
	
	if (THUserWidget)
	{
		THUserWidget->SetOwningActor(GetOwner());
	}
}
