// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GPCharacterNPC.generated.h"

UENUM(BlueprintType)
enum class ENPCType : uint8
{
	NONE       UMETA(DisplayName = "None"),
	GSSHOP     UMETA(DisplayName = "GSShop"),
	SUITSHOP   UMETA(DisplayName = "SuitShop"),
	JUICESHOP  UMETA(DisplayName = "JuiceShop"),
	QUEST      UMETA(DisplayName = "QuestNPC")
};

/**
 * 
 */
UCLASS()
class GP2025_API AGPCharacterNPC : public ACharacter
{
	GENERATED_BODY()


public:
	AGPCharacterNPC();
	virtual void BeginPlay() override;
	
public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	ENPCType NPCType = ENPCType::NONE;


	// <Shop>
	UPROPERTY()
	UUserWidget* ShopWidget;

	UPROPERTY()
	TSubclassOf<UUserWidget> ShopWidgetClass;
	
	void OpenShopUI(APlayerController* PlayerController);
	void CloseShopUI();

	// <Interact with players>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* InteractionSphere;

	UFUNCTION()
	void OnInteractionStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void CheckAndHandleInteraction(AGPCharacterMyplayer* MyPlayer);

	UFUNCTION()
	void ExitInteraction();

	UPROPERTY()
	bool bIsInteracting = false;

	// <Description UI>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* InteractionWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> WBPClass_NPCInteraction;


	// <Quest>
	UPROPERTY()
	UUserWidget* QuestWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> QuestWidgetClass;

	FTimerHandle QuestOpenUITimerHandle;

	UFUNCTION()
	void OpenQuestUIDelayed();

	void OpenQuestUI(APlayerController* PlayerController);
	void CloseQuestUI();
};
