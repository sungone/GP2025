// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPMagicBat.generated.h"

UCLASS()
class GP2025_API AGPMagicBat : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPMagicBat();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Collision")
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Damage")
	float Damage = 25.f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void AttachToPlayer(class AGPCharacterBase* PlayerCharacter, FName SocketName);

};
