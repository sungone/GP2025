// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "GPCharacterPlayer.generated.h"


struct FGPItemStruct;

/**
 *
 */
UCLASS()

class GP2025_API AGPCharacterPlayer : public AGPCharacterBase
{
	GENERATED_BODY()

public:
	AGPCharacterPlayer();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
protected:
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterControlData) override;
	virtual void SetCharacterType(ECharacterType NewCharacterControlType) override;

	virtual USkeletalMeshComponent* GetCharacterMesh() const override;
	
	// Character Mesh Section
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts")
	TObjectPtr<USkeletalMeshComponent> HeadMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts")
	TObjectPtr<USkeletalMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts")
	TObjectPtr<USkeletalMeshComponent> LegMesh;

	void SetupMasterPose();
	void ApplyCharacterPartsFromData(const class UGPCharacterControlData* CharacterData);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Helmet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AGPWeaponBase> WeaponActor;

	void EquipWeaponFromData(const class UGPCharacterControlData* CharacterData);

public :
	// Inventory Equip Item Change
	UFUNCTION(BlueprintCallable, Category = "Item")
	void EquipItemOnCharacter(struct FGPItemStruct& ItemData);
};
