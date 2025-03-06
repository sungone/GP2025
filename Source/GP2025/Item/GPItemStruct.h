
#pragma once

#include "CoreMinimal.h"
#include "../../GP_Server/Source/Common/Type.h"
#include "GPItemStruct.generated.h"


UENUM()
enum class ECategory : uint8
{
	bow UMETA(DisplayName = "Bow"),
	sword UMETA(DisplayName = "Sword"),
	helmet UMETA(DisplayName = "Helmet"),
	chest UMETA(DisplayName = "Chest"),
	consumable UMETA(DisplayName = "Consumable"),
	Gold UMETA(DisplayName = "Gold"),
	Quest UMETA(DisplayName = "Quest"),
	None UMETA(DisplayName = "None")
};

UENUM()
enum class EAbilityType : uint8
{
	None UMETA(DisplayName = "None"),
	Recover UMETA(DisplayName = "Recover"),
	AtkBuff UMETA(DisplayName = "Attack Buff"),
	EXP UMETA(DisplayName = "EXP Boost"),
	Gold UMETA(DisplayName = "Gold Bonus")
};

USTRUCT(BlueprintType)
struct FGPItemStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public :
	FGPItemStruct();
public :

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	ECategory Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Hp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float CrtRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float DodgeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EAbilityType AType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float AValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Grade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	uint8 bIsMonster : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ResellPrice;

	// 상점에서 살 때 가격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Price;

	// 되팔 수 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	uint8 bIsSellable : 1;

	// 살수있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	uint8 bIsBuy : 1;

	// 아이템 썸네일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemThumbnail;

	// 아이템 스태틱 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMesh* ItemStaticMesh;

	// 아이템 스켈레탈 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	USkeletalMesh* ItemSkeletalMesh;

	// 무기 클래스일 때 지정
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Item")
	TSubclassOf<class AGPWeaponBase> WeaponClass;
};
