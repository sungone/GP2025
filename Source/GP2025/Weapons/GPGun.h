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

	// 총알 발사 기능
	void FireBullet();
};
