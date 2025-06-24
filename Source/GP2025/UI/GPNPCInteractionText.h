// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/GPCharacterNPC.h"
#include "GPNPCInteractionText.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPNPCInteractionText : public UUserWidget
{
	GENERATED_BODY()
	
public :

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NPCText;

};
