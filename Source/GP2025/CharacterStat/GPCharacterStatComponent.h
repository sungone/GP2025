// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../GP_Server/Proto.h"
#include "GPCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP2025_API UGPCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGPCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public :

	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

	FORCEINLINE float GetMaxHp() { return MaxHp; }
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }
	float ApplyDamage(float InDamage);

	FORCEINLINE float GetDamage() { return Attack; }
	FORCEINLINE void SetDamage(float attack) { Attack = attack; }

	FORCEINLINE void SetMaxHp(float Hp) { MaxHp = Hp; }
	FORCEINLINE void SetCurrentHp(float Hp) { CurrentHp = Hp; }
	void SetHp(float NewHp);

protected :


	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	int Lv;

	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float MaxHp;

	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float Attack;

	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float CrtRate;

	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float CrtValue;

	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float MoveSpeed;

	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float Dodge;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat")
	float CurrentHp;
};
