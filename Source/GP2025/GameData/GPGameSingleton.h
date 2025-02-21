// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPCharacterPlayerStat.h"
#include "GPGameSingleton.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGPGameSingleton, Error, All);

/**
 * 
 */
UCLASS()
class GP2025_API UGPGameSingleton : public UObject
{
	GENERATED_BODY()
	
public :
	UGPGameSingleton();
	static UGPGameSingleton& Get();

// Character Stat Data Section
public :
	FORCEINLINE FGPCharacterPlayerStat GetCharacterPlayerStat(int32 InLevel) const {
		return CharacterPlayerStatTable.IsValidIndex(InLevel) ? CharacterPlayerStatTable[InLevel] : FGPCharacterPlayerStat();
	}

	UPROPERTY()
	int32 CharacterPlayerMaxLevel;
private :
	TArray<FGPCharacterPlayerStat> CharacterPlayerStatTable;
};
