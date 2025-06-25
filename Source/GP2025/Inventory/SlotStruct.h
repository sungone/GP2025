#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EItemTypes.h" 
#include "SlotStruct.generated.h"


/** �κ��丮 ���� ������ �����ϴ� ����ü */
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