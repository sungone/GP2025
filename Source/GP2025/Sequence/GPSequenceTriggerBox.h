// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/Actor.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "GPSequenceTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPSequenceTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
	
public:
    AGPSequenceTriggerBox();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(
        AActor* OverlappedActor,
        AActor* OtherActor);

public:
    /** ������ ���� ������ ���� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
    ULevelSequence* LevelSequenceAsset;

private:
    /** ���� ������ ���� */
    UPROPERTY()
    ALevelSequenceActor* SequenceActor;

    /** ���� ������ �÷��̾� */
    UPROPERTY()
    ULevelSequencePlayer* SequencePlayer;
};
