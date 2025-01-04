
#pragma once

#include "CoreMinimal.h"
#include "GPItemStruct.generated.h"

UENUM()
enum class EItemCategoryType : uint8
{
	Weapon ,
	Armor ,
	Usable ,
	Unusable ,
	NONE
};

UENUM()
enum class EItemDetailType : uint8
{
	Weapon,
	Armor,
	Usable,
	Unusable ,
	NONE
};

UENUM()
enum class EItemAbilityType : uint8
{
	Recover ,
	AttackBuff ,
	NONE
};

USTRUCT(BlueprintType)
struct FGPItemStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public :
	FGPItemStruct();
public :
	// ������ ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemIndex;

	// ������ �̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemName;

	// ������ ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Amount;

	// ������ �Ž�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMesh* ItemMesh;

	// ������ ī�װ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemCategoryType Category;

	// ������ Ÿ�� ���� ���� - ���� ����� ��쿡 ���� ���� ���п�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemDetailType DetailType;

	// ���ݷ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Attack;

	// ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Hp;

	// ũ��Ƽ�� Ȯ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float CriticalRate;

	// ȸ�� Ȯ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float DodgeRate;

	// �̵� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float MoveSpeed;

	// Ư�� ��� Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemAbilityType AbilityType;

	// Ư�� ��� Ÿ�� - Ÿ�� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float AbilityValue;

	// ������ ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Grade;

	// ���ͷ� ��� �Ҽ��ֳ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	uint8 bIsMonster : 1;

	// ���� �� �ִ��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	uint8 bCanResell : 1;

	// ����ִ��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	uint8 bIsBuy : 1;

	// �������� �� �� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Price;
};
