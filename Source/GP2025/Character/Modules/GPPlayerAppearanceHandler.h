// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPPlayerAppearanceHandler.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPPlayerAppearanceHandler : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(class AGPCharacterPlayer* InOwner);
	void ApplyCharacterPartsFromData(const class UGPCharacterControlData* CharacterData);
	void SetupLeaderPose();
	void AttachWeaponToBodyMesh();
	void EquipWeaponFromData(const class UGPCharacterControlData* CharacterData);
	USkeletalMesh* GetBodyMeshByCharacterType(const FGPItemStruct& ItemData, uint8 CurrentCharacterType);

	UFUNCTION(BlueprintCallable)
	void EquipItemOnCharacter(struct FGPItemStruct& ItemData);

	UFUNCTION(BlueprintCallable)
	void UnequipItemFromCharacter(ECategory Category);

private:
	UPROPERTY()
	class AGPCharacterPlayer* Owner;
};
