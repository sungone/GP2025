// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGPCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHpChanged, float, NewHpRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNickNameChanged, FString, NewNickName);

UCLASS()
class GP2025_API AGPCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AGPCharacterBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// <CharacterType>
	TMap<ECharacterType, class UGPCharacterControlData*> CharacterTypeManager;
	ECharacterType& CurrentCharacterType = CharacterInfo.CharacterType;

	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterData);
	virtual void SetCharacterType(ECharacterType NewCharacterType);

	// <CharacterInfo>
	virtual void SetCharacterInfo(const FInfoData& CharacterInfo_);
	virtual void SetNameByCharacterInfo();
	FInfoData CharacterInfo;

	// <Move Sync>
	void HandleAutoAttackState();
	void HandleQSkillState();
	void HandleESkillState();
	void HandleRSkillState();
	void HandleRemoteMovementSync(float DeltaTime);
	void HandleRemoteJumpSync();
	float Ground_ZLocation = 162.55f;
	void SetGroundZLocation(float Z);
	void UpdateGroundZLocation();
	float GroundTraceElapsedTime = 0.f;
	const float GroundTraceInterval = 1.f;

	// <Appearance>
	virtual USkeletalMeshComponent* GetCharacterMesh() const;

	// <Combat>
	UPROPERTY()
	class UGPCharacterCombatHandler* CombatHandler;

	// <UI>
	UPROPERTY()
	class UGPCharacterUIHandler* UIHandler;

	// <Stat Delegate>
	FOnHpChanged OnHpChanged;
	FOnLevelChanged OnLevelChanged;
	FOnNickNameChanged OnNickNameChanged;


};
