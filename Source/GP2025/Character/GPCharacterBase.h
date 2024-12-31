// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../GP_Server/Proto.h"
#include "GPCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Warrior ,
	Gunner
};

UCLASS()
class GP2025_API AGPCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AGPCharacterBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 서버에서 받은 다른 클라이언트 정보를 업데이트
	void SetClientInfoFromServer(FPlayerInfo& PlayerInfo_);

public:
	FPlayerInfo PlayerInfo;

// 기본 공격 애니메이션 및 공격 애니메이션 몽타주 코드
public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> AutoAttackActionMontage;

	void ProcessAutoAttackCommand();
	void OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	bool bIsAutoAttacking = false;

// Control Data 세팅
protected :
	virtual void SetCharacterControlData(const class UGPCharacterControlData* CharacterControlData);

	UPROPERTY(EditAnywhere, Category = "CharacterControl", Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UGPCharacterControlData*> CharacterControlManager;
};
