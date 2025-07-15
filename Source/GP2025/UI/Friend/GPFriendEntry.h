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
    virtual void NativeConstruct() override;

public:
    void SetFriendInfo(uint32 Id , const FString& NickName, int32 Level, bool bOnline = false);

    UFUNCTION()
    void OnEntryClicked();
public:

    UPROPERTY(meta = (BindWidget))
    class UButton* EntryButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* NicknameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* LevelText;

    UPROPERTY()
    uint32 FriendUserID;

    bool bIsOnline = false;

    UPROPERTY()
    class UGPFriendBox* OwnerBox; // Parent box widget
	
};

