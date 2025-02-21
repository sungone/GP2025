// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/GPGameSingleton.h"

DEFINE_LOG_CATEGORY(LogGPGameSingleton);

UGPGameSingleton::UGPGameSingleton()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/GameData/GPCharacterPlayerStatTable.GPCharacterPlayerStatTable'"));
	if (nullptr != DataTableRef.Object)
	{
		const UDataTable* DataTable = DataTableRef.Object;
		check(DataTable->GetRowMap().Num() > 0);

		TArray<uint8*> ValueArray;
		DataTable->GetRowMap().GenerateValueArray(ValueArray);
		Algo::Transform(ValueArray, CharacterPlayerStatTable,
			[](uint8* Value)
			{
				return *reinterpret_cast<FGPCharacterPlayerStat*>(Value);
			}
		);
	}

	CharacterPlayerMaxLevel = CharacterPlayerStatTable.Num();
	ensure(CharacterPlayerMaxLevel > 0);
}

UGPGameSingleton& UGPGameSingleton::Get()
{
	UGPGameSingleton* Singleton = CastChecked<UGPGameSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		return *Singleton;
	}

	UE_LOG(LogGPGameSingleton, Error, TEXT("Invalid Game Singleton"));
	return *NewObject<UGPGameSingleton>();
}
