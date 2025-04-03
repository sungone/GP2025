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

	// 서버에서 받은 다른 클라이언트 정보를 업데이트
	virtual void SetCharacterInfo(FInfoData& CharacterInfo_);
	FInfoData CharacterInfo;

	// 플레이어 -> 마스터 포즈 컴포넌트 (BodyMesh) , 몬스터 -> GetMesh()
	virtual USkeletalMeshComponent* GetCharacterMesh() const;

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

// 스킬 공통 기능
 	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

// 남자 스킬   
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

// 여자 스킬
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
