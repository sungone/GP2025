#pragma once

#include "CoreMinimal.h"
#include "EItemTypes.generated.h"


UENUM(BlueprintType)
enum class EItemTypes : uint8
{
    Weapon     UMETA(DisplayName = "Weapon"),
    Armor      UMETA(DisplayName = "Armor"),
    Eatables   UMETA(DisplayName = "Eatables")
};