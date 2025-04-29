// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPCharacterSelectButtonWidget.generated.h"

UENUM(BlueprintType)
enum class ELobbyCharacterSelectType : uint8
{
	MAN,
	WOMAN
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSelected, ELobbyCharacterSelectType, SelectedCharacter);


/**
 * 
 */
UCLASS()
class GP2025_API UGPCharacterSelectButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public :
	virtual void NativeConstruct() override;


public :
	UPROPERTY(meta = (BindWidget))
	class UButton* SelectButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	ELobbyCharacterSelectType CharacterType;

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCharacterSelected OnCharacterSelected;

public :

	void HandleClicked();
};
