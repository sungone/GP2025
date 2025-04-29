// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "Item/GPItemStruct.h"
#include "GPCharacterPlayer.generated.h"


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

	// <CharacterType>
public:
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterControlData) override;
	virtual void SetCharacterType(ECharacterType NewCharacterControlType) override;
	UFUNCTION(BlueprintCallable, Category = "CharacterType")
	virtual bool bIsGunnerCharacter() const;

	// <Appearance>
public :
	UPROPERTY()
	class UGPPlayerAppearanceHandler* AppearanceHandler;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts")
	TObjectPtr<USkeletalMeshComponent> HeadMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts")
	TObjectPtr<USkeletalMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts")
	TObjectPtr<USkeletalMeshComponent> LegMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Helmet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AGPWeaponBase> WeaponActor;

	virtual USkeletalMeshComponent* GetCharacterMesh() const override;
	bool bDidLeaderPoseSetup = false;

	// <Equipment Tracking>
	UPROPERTY()
	TMap<ECategory, int32> EquippedItemIDs;
};