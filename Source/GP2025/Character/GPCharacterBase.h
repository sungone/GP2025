// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGPCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHpChanged, float, NewHpRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpChanged, float, NewExpRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewGold);
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
public:
	TMap<ECharacterType, class UGPCharacterControlData*> CharacterTypeManager;
	ECharacterType CurrentCharacterType;

	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterData);
	virtual void SetCharacterType(ECharacterType NewCharacterType);

	// <CharacterInfo>
	virtual void SetCharacterInfo(FInfoData& CharacterInfo_);
	FInfoData CharacterInfo;

	// <Appearance>
	virtual USkeletalMeshComponent* GetCharacterMesh() const;


	// <UI>
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> HpBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> LevelText;

	UUserWidget* HpBarWidget;
	UUserWidget* LevelTextWidget;

	FOnHpChanged OnHpChanged;
	FOnExpChanged OnExpChanged;
	FOnLevelChanged OnLevelChanged;

	UGPWidgetComponent* CreateWidgetComponent(const FString& Name, const FString& WidgetPath, FVector Location, FVector2D Size, UUserWidget*& OutUserWidget);

	// Gold Setting
public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnGoldChanged OnGoldChanged;

	// Nick Name Setting
public:
	UUserWidget* NickNameWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGPWidgetComponent> NickNameText;

	FOnNickNameChanged OnNickNameChanged;

	// <Combat Handler>
public :
	UPROPERTY()
	class UGPCharacterCombatHandler* CombatHandler;
};
