// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "THWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class TIGERHUNTER_API UTHWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
protected :
	virtual void InitWidget() override;
};
