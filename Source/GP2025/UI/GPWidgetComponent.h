// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GPWidgetComponent.generated.h"

/**
 * 3D �������� UI�� ��ġ�� ��
 */

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP2025_API UGPWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetComponent(FVector Location, FVector2D Size);
protected :
	virtual void InitWidget() override;
};
