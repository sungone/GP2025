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
	void SetupMasterPose();
	void EquipWeaponFromData(const class UGPCharacterControlData* CharacterData);
	
	UFUNCTION(BlueprintCallable)
	void EquipItemOnCharacter(struct FGPItemStruct& ItemData);

private:
	UPROPERTY()
	class AGPCharacterPlayer* Owner;
};
