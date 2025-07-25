#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "GPSequenceManager.generated.h"

DECLARE_DELEGATE(FOnSequenceFinishedDelegate);


USTRUCT(BlueprintType)
struct FNamedSequence
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SequenceName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULevelSequence* SequenceAsset;
};

UCLASS(Blueprintable)
class GP2025_API UGPSequenceManager : public UObject
{
	GENERATED_BODY()
public:
	FOnSequenceFinishedDelegate OnSequenceFinishedDelegate;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequences")
	TArray<FNamedSequence> SequenceList;

	UFUNCTION()
	void PlaySequenceByName(UObject* WorldContext, FName SequenceName);
	UFUNCTION()
	void SkipSequence();
	UFUNCTION()
	bool IsSequencePlaying() const;
	UFUNCTION()
	void OnSequenceFinished();

private:
	UPROPERTY()
	ULevelSequencePlayer* SequencePlayer;

	UPROPERTY()
	ALevelSequenceActor* SequenceActor;
};
