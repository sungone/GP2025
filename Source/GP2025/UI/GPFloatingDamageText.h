// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPFloatingDamageText.generated.h"

UCLASS()
class GP2025_API AGPFloatingDamageText : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPFloatingDamageText();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetDamageText(float DamageAmount , bool bIsCrt);

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* DamageWidgetComponent;

	UPROPERTY()
	class UTextBlock* DamageText;


	FTimerHandle DestroyTimerHandle;
	
	void DestroySelf();

};
