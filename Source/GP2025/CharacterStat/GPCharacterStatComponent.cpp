// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/GPCharacterStatComponent.h"

// Sets default values for this component's properties
UGPCharacterStatComponent::UGPCharacterStatComponent()
{
	MaxHp = 200.f;
	Attack = 50.f;
	CurrentHp = MaxHp;
}


// Called when the game starts
void UGPCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
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

