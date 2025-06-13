// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPFloatingQuestText.generated.h"

UCLASS()
class GP2025_API AGPFloatingQuestText : public AActor
{
	GENERATED_BODY()

public:
	AGPFloatingQuestText();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ����Ʈ �޽��� ���� �Լ�
	void SetQuestMessage(const FString& Message);

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* QuestWidgetComponent;

	UPROPERTY()
	class UTextBlock* QuestMessageText;

	FTimerHandle DestroyTimerHandle;
	void DestroySelf();

};
