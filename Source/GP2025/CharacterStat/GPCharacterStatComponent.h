// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../GP_Server/Source/Common/Proto.h"
#include "GPCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float);

DECLARE_MULTICAST_DELEGATE(FOnExpFullDelegete);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnExpChangedDelegate, float);

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

	FOnExpFullDelegete OnExpFull;
	FOnExpChangedDelegate OnExpChanged;

	FORCEINLINE float GetMaxHp() { return MaxHp; }
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }
	float ApplyDamage(float InDamage);

	FORCEINLINE float GetDamage() { return Attack; }
	FORCEINLINE void SetDamage(float attack) { Attack = attack; }

	FORCEINLINE void SetMaxHp(float Hp) { MaxHp = Hp; }
	FORCEINLINE void SetCurrentHp(float Hp) { CurrentHp = Hp; }
	void SetHp(float NewHp);


	FORCEINLINE float GetCrtRate() { return CrtRate; }
	FORCEINLINE void SetCrtRate(float crtRate) { CrtRate = crtRate; }

	FORCEINLINE float GetCrtValue() { return CrtValue; }
	FORCEINLINE void SetCrtValue(float crtValue) { CrtValue = crtValue; }

	FORCEINLINE float GetDodge() { return Dodge; }
	FORCEINLINE void SetDodge(float dodge) { Dodge = dodge; }

	FORCEINLINE float GetMaxExp() { return MaxExp; }
	FORCEINLINE void SetMaxExp(float Exp) { MaxExp = Exp; }

	FORCEINLINE float GetCurrentExp() { return CurrentExp; }
	FORCEINLINE void SetCurrentExp(float Exp) { CurrentHp = Exp; }
	float AddExp(float InExp);

	void SetExp(float NewExp);

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


	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float MaxExp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat")
	float CurrentExp;
};
