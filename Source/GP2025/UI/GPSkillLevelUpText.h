// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPSkillLevelUpText.generated.h"

UCLASS()
class GP2025_API AGPSkillLevelUpText : public AActor
{
	GENERATED_BODY()
	

public:
	AGPSkillLevelUpText();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void SetSkillLevelUpText(const FString& Message);

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* SkillTextWidgetComponent;

	UPROPERTY()
	class UTextBlock* SkillTextBlock;

private:
	FTimerHandle DestroyTimerHandle;
	void DestroySelf();
};
