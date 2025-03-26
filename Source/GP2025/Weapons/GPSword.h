// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/GPWeaponBase.h"
#include "GPSword.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPSword : public AGPWeaponBase
{
	GENERATED_BODY()
	
public:
	AGPSword();

	virtual void AttackHitCheck() override;
};
