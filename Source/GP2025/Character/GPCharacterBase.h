// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/GPAnimationAttackInterface.h"
#include "Interface/GPCharacterItemInterface.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGPCharacter, Log, All);
DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UGPItemData*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHpChanged, float, NewHpRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpChanged, float, NewExpRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, NewLevel);

USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()
	FTakeItemDelegateWrapper() {}
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) : ItemDelegate(InItemDelegate) {}
	FOnTakeItemDelegate ItemDelegate;
};

UCLASS()
class GP2025_API AGPCharacterBase : public ACharacter ,public IGPAnimationAttackInterface , public IGPCharacterItemInterface
{
	GENERATED_BODY()

public:
	AGPCharacterBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// 서버에서 받은 다른 클라이언트 정보를 업데이트
	void SetCharacterInfo(FInfoData& CharacterInfo_);
	FInfoData CharacterInfo;

// 기본 공격 애니메이션 및 공격 애니메이션 몽타주 코드
public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> AttackActionMontage;

	void ProcessAutoAttackCommand();
	void OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	bool bIsAutoAttacking = false;

// Control Data 세팅
public :
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterData);
	virtual void SetCharacterType(ECharacterType NewCharacterType);

	TMap<ECharacterType, class UGPCharacterControlData*> CharacterTypeManager;

	ECharacterType CurrentCharacterType;

// Attack Hit Section
protected :
	virtual void AttackHitCheck() override;

// UI Widget Section
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> HpBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> ExpBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> LevelText;

	FOnHpChanged OnHpChanged;
	FOnExpChanged OnExpChanged;
	FOnLevelChanged OnLevelChanged;

	UGPWidgetComponent* CreateWidgetComponent(const FString& Name, const FString& WidgetPath, FVector Location, FVector2D Size);

// Dead Section
public :
	virtual void SetDead();
	void PlayDeadAnimation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;
	float DeadEventDelayTime = 0.5f;


// Character Mesh Section
protected :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts")
	TObjectPtr<USkeletalMeshComponent> HeadMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts")
	TObjectPtr<USkeletalMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts")
	TObjectPtr<USkeletalMeshComponent> LegMesh;

	void SetupMasterPose();
	void ApplyCharacterPartsFromData(const class UGPCharacterControlData* CharacterData);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment" , Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Chest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Helmet;

// Item Section
protected:
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;

	virtual void TakeItem(class UGPItemData* InItemData) override;
	virtual void DrinkPotion(class UGPItemData* InItemData);
	virtual void EquipChest(class UGPItemData* InItemData);
	virtual void EquipHelmet(class UGPItemData* InItemData);
	virtual void AddExp(class UGPItemData* InItemData);

};
