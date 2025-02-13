#pragma once

#include "CoreMinimal.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Templates/SubclassOf.h"
#include "Containers/Map.h"
#include "Containers/Array.h"

namespace
{
	// ���� ������� static �����ߴµ� �׷��� ���� �۾��� �� �ҷ������� ���� ������ �� ���� �� �� -> �׷��� ó���� �ε� �������ǰ�..?
	// (CSV + DataTable Ȱ��) DataTable�� csv���� �Ľ��ؼ� ���� ������� �����غ���

	template <typename T>
	T* LoadAsset(const FString& Path)
	{
		ConstructorHelpers::FObjectFinder<T> AssetRef(*Path);
		return AssetRef.Object;
	}

	template <typename T>
	TSubclassOf<T> LoadClass(const FString& Path)
	{
		ConstructorHelpers::FClassFinder<T> ClassRef(*Path);
		return ClassRef.Class;
	}

	template <typename EnumType>
	void LoadCharacterData(TMap<EnumType, UGPCharacterControlData*>& Manager, const TArray<TTuple<EnumType, FString>>& DataArray)
	{
		for (const auto& Data : DataArray)
		{
			ConstructorHelpers::FObjectFinder<UGPCharacterControlData> DataRef(*Data.Value);
			if (DataRef.Object)
			{
				Manager.Add(Data.Key, DataRef.Object);
			}
		}
	}
}