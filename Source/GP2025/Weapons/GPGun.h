// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/GPWeaponBase.h"
#include "GPGun.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPGun : public AGPWeaponBase
{
	GENERATED_BODY()
	
public:
	AGPGun();

	virtual void StartAttack() override;

	virtual void AttackHitCheck() override;

	float ValidRange = 5000.f;
};
