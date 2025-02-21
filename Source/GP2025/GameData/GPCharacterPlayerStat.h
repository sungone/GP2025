
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GPCharacterPlayerStat.generated.h"

USTRUCT(BlueprintType)
struct FGPCharacterPlayerStat : public FTableRowBase
{
	GENERATED_BODY()

public :
	FGPCharacterPlayerStat() : MaxHp(0.f), Attack(0.f), CrtRate(0.f), CrtValue(0.f), Dodge(0.f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Attack;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float CrtRate;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float CrtValue;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Dodge;

	FGPCharacterPlayerStat operator+(const FGPCharacterPlayerStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		FGPCharacterPlayerStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		int32 StatNum = sizeof(FGPCharacterPlayerStat) / sizeof(float);

		for (int32 i = 0; i < StatNum; ++i)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}

		return Result;
	}
};