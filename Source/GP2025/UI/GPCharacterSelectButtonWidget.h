// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../GP_Server/Source/Common/Type.h"
#include "GPCharacterSelectButtonWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSelected, uint8, SelectedCharacter);

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
	uint8  CharacterType;

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCharacterSelected OnCharacterSelected;

public :

	UFUNCTION()
	void HandleClicked();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ClickSound;
};
