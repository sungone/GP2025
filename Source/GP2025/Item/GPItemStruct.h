
#pragma once

#include "CoreMinimal.h"
#include "../../GP_Server/Source/Shared/Type.h"
#include ""
#include "GPItemStruct.generated.h"

UENUM()
enum class EItemCategoryType : uint8
{
	weapon ,
	armor ,
	useable ,
	unuseable ,
	NONE
};

UENUM()
enum class EItemDetailType : uint8
{
	bow,
	sword,
	helmet,
	chest,
	consumable,
	EXP ,
	Gold ,
	Quest ,
	NONE
};

UENUM()
enum class EItemAbilityType : uint8
{
	recove,
	atc_buff,
	EXP,
	Gold ,
	NONE
};

USTRUCT(BlueprintType)
struct FGPItemStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public :
	FGPItemStruct();
public :
	// 아이템 구분자
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemIndex;

	// 아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	// 아이템 카테고리 구분
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemCategoryType Category;

	// 아이템 타입 하위 구분 - 장착 장비일 경우에 장착 부위 구분용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemDetailType DetailType;

	// 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Attack;

	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Hp;

	// 크리티컬 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float CriticalRate;

	// 회피 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float DodgeRate;

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float MoveSpeed;

	// 특수 기술 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemAbilityType AbilityType;

	// 특수 기술 타입 - 타입 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float AbilityValue;

	// 아이템 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Grade;

	// 몬스터로 드랍 할수있나 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	uint8 bIsMonster : 1;

	// 상점에서 살 때 가격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ResellPrice;

	// 상점에서 살 때 가격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Price;

	// 되팔 수 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	uint8 bCanResell : 1;

	// 살수있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	uint8 bIsBuy : 1;

	// 아이템 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDescription;

	// 아이템 양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemStackSize;

	// 아이템 썸네일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemThumbnail;

	// 아이템 매쉬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMesh* ItemMesh;
};
