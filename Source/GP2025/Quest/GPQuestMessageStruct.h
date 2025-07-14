#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GPQuestMessageStruct.generated.h"

USTRUCT(BlueprintType)
struct FGPQuestMessageStruct : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    uint8 QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText QuestMessage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText QuestTitle;
};