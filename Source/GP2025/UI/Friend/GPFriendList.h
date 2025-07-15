// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPFriendList.generated.h"

class UScrollBox;
class UGPFriendEntry;

/**
 * 
 */
UCLASS()
class GP2025_API UGPFriendList : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    void ClearFriendEntries();
    void AddFriendEntry(const FString& NickName, int32 Level);

protected:
    UPROPERTY(meta = (BindWidget))
    UScrollBox* FriendScrollBox;

    UPROPERTY(EditDefaultsOnly, Category = "Friend UI")
    TSubclassOf<UGPFriendEntry> FriendEntryClass;
};
