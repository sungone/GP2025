// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GPCharacterNPC.generated.h"

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
	UPROPERTY()
	UUserWidget* ShopWidget;

	UPROPERTY()
	TSubclassOf<UUserWidget> ShopWidgetClass;

	void OpenShopUI(APlayerController* PlayerController);
	void CloseShopUI();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* InteractionSphere;

	UFUNCTION()
	void OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
