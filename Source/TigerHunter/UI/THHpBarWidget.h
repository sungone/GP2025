// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "THUserWidget.h"
#include "THHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class TIGERHUNTER_API UTHHpBarWidget : public UTHUserWidget
{
	GENERATED_BODY()
	
public :
	UTHHpBarWidget(const FObjectInitializer& ObjectInitializer);

protected :
	virtual void NativeConstruct() override;

public :
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	void UpdateHpBar(float NewCurrentHp);
protected :
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
