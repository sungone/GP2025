// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPFriendEntry.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPFriendEntry : public UUserWidget
{
	GENERATED_BODY()

public:
    void SetFriendInfo(const FString& NickName, int32 Level);

public:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* NicknameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* LevelText;
	
};

