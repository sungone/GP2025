#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EItemTypes.h" 
#include "SlotStruct.generated.h"


/** 인벤토리 슬롯 정보를 저장하는 구조체 */
USTRUCT(BlueprintType)
struct FSlotStruct
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TArray<int32> ItemUniqueIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FDataTableRowHandle ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemTypes ItemType;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    //int32 ItemUniqueID;

    FSlotStruct()
        : Quantity(0), ItemType(EItemTypes::Weapon) {}
};