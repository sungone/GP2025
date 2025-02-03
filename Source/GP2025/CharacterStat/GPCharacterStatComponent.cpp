// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/GPCharacterStatComponent.h"

// Sets default values for this component's properties
UGPCharacterStatComponent::UGPCharacterStatComponent()
{
	MaxHp = 100.f;
	Attack = 50.f;
	CurrentHp = MaxHp;
	MaxExp = 100.f;
}

// Called when the game starts
void UGPCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHp = MaxHp;
}

float UGPCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UGPCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.f, MaxHp);

	OnHpChanged.Broadcast(CurrentHp);
}

float UGPCharacterStatComponent::AddExp(float InExp)
{
	const float PrevExp = CurrentExp;
	const float ActualExp = FMath::Clamp<float>(InExp, 0, InExp);

	SetExp(PrevExp + ActualExp);
	if (CurrentExp >= MaxExp)
	{
		OnExpFull.Broadcast();
	}

	return CurrentExp;
}

void UGPCharacterStatComponent::SetExp(float NewExp)
{
	CurrentExp = FMath::Clamp<float>(NewExp, 0.0f, MaxExp);

	OnExpChanged.Broadcast(CurrentExp);
}

