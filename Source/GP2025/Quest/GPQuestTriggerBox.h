// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Engine/DataTable.h"
#include "GPQuestMessageStruct.h"
#include "GPQuestTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPQuestTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AGPQuestTriggerBox();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	uint8 TriggerQuestType;
	
public :
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	UDataTable* QuestMessageTable;
};
