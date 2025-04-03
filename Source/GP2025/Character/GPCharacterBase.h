// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/GPAnimationAttackInterface.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGPCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHpChanged, float, NewHpRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpChanged, float, NewExpRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, NewLevel);


UCLASS()
class GP2025_API AGPCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AGPCharacterBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// �������� ���� �ٸ� Ŭ���̾�Ʈ ������ ������Ʈ
	virtual void SetCharacterInfo(FInfoData& CharacterInfo_);
	FInfoData CharacterInfo;

	// �÷��̾� -> ������ ���� ������Ʈ (BodyMesh) , ���� -> GetMesh()
	virtual USkeletalMeshComponent* GetCharacterMesh() const;

// �⺻ ���� �ִϸ��̼� �� ���� �ִϸ��̼� ��Ÿ�� �ڵ�
public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> AttackActionMontage;

	void ProcessAutoAttackCommand();
	void OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	bool bIsAutoAttacking = false;

// Control Data ����
public :
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterData);
	virtual void SetCharacterType(ECharacterType NewCharacterType);

	TMap<ECharacterType, class UGPCharacterControlData*> CharacterTypeManager;
	ECharacterType CurrentCharacterType;

// UI Widget Section
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> HpBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> LevelText;

	UUserWidget* HpBarWidget;
	UUserWidget* LevelTextWidget;

	FOnHpChanged OnHpChanged;
	FOnExpChanged OnExpChanged;
	FOnLevelChanged OnLevelChanged;

	UGPWidgetComponent* CreateWidgetComponent(const FString& Name, const FString& WidgetPath, FVector Location, FVector2D Size, UUserWidget*& OutUserWidget);

// Dead Section
public :
	virtual void SetDead();
	void PlayDeadAnimation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;
	float DeadEventDelayTime = 0.5f;

// ��ų ���� ���
 	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

// ���� ��ų   
	void ProcessHitHardCommand();
	void ProcessClashCommand();
	void ProcessWhirlwindCommand();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> HitHardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> ClashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> WhirlwindMontage;

	bool bIsUsingSkill = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "FX")
	class UNiagaraSystem* HitHardEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "FX")
	class UNiagaraSystem* ClashEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "FX")
	class UNiagaraSystem* WhirlwindEffect;

// ���� ��ų
	void ProcessThrowingCommand();
	void ProcessFThrowingCommand();
	void ProcessAngerCommand();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> ThrowingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> FThrowingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> AngerMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	class UNiagaraSystem* AngerEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Effect")
	TSubclassOf<AActor> ProjectileEffectClass;
};
