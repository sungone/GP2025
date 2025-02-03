// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/GPAnimationAttackInterface.h"
#include "Interface/GPCharacterWidgetInterface.h"
#include "../../GP_Server/Source/Common/Proto.h"
#include "GPCharacterBase.generated.h"


UCLASS()
class GP2025_API AGPCharacterBase : public ACharacter , public IGPAnimationAttackInterface ,public IGPCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	AGPCharacterBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// �������� ���� �ٸ� Ŭ���̾�Ʈ ������ ������Ʈ
	void SetCharacterInfoFromServer(FInfoData& CharacterInfo_);
	FInfoData CharacterInfo;
	

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

// Attack Hit Section
protected :
	virtual void AttackHitCheck() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	float CalculateDamage();

// Stat Section
public :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat" , Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPCharacterStatComponent> Stat;


// UI Widget Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> HpBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> ExpBar;

	virtual void SetupCharacterWidget(class UGPUserWidget* InUserWidget) override;

// Dead Section
public :
	virtual void SetDead();
	void PlayDeadAnimation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;
	float DeadEventDelayTime = 0.5f;

// Equip Item Section
public : 
	void EquipHelmet(USkeletalMesh* HelmetMesh);
	void UnequipHelmet();

	void EquipChest(USkeletalMesh* ChestMesh);
	void UnequipChest();

	void EquipWeapon(USkeletalMesh* WeaponMesh);
	void UnequipWeapon();

	void EquipPants(USkeletalMesh* PantsMesh);
	void UnequipPants();

	void EquipItemFromDataAsset(UGPCharacterControlData* CharacterData);

	UPROPERTY(VisibleAnywhere, Category = "Equip")
	TObjectPtr<USkeletalMeshComponent> HelmetMeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Equip")
	TObjectPtr<USkeletalMeshComponent> ChestMeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Equip")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Equip")
	TObjectPtr<USkeletalMeshComponent> PantsMeshComp;
};
