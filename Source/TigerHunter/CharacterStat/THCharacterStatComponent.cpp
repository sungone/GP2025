// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/THCharacterStatComponent.h"

// Sets default values for this component's properties
UTHCharacterStatComponent::UTHCharacterStatComponent()
{
	MaxHp = 200.f;
	CurrentHp = MaxHp;
}


// Called when the game starts
void UTHCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	SetHp(MaxHp);
}

float UTHCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(FMath::Clamp<float>(PrevHp - ActualDamage, 0.0f, MaxHp));

	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UTHCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.f, MaxHp);
	OnHpChanged.Broadcast(CurrentHp);
}


