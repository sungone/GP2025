// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GPCubeHISMActor.generated.h"

UCLASS()
class GP2025_API AGPCubeHISMActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPCubeHISMActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
    virtual void OnConstruction(const FTransform& Transform) override;

public:
    UPROPERTY(EditAnywhere, Category = "HISM")
    UHierarchicalInstancedStaticMeshComponent* HISMComponent;

    UPROPERTY(EditAnywhere, Category = "HISM")
    UStaticMesh* MeshToUse;

    UPROPERTY(EditAnywhere, Category = "HISM Placement")
    FVector StartLocation;

    UPROPERTY(EditAnywhere, Category = "HISM Placement")
    FVector NextLocation;

    UPROPERTY(EditAnywhere, Category = "HISM Placement", meta = (ClampMin = "1"))
    int32 InstanceCount;

    UPROPERTY(EditAnywhere, Category = "HISM Placement")
    FVector InstanceScale;

    UPROPERTY(EditAnywhere, Category = "HISM Placement")
    FRotator StartRotation;

    UPROPERTY(EditAnywhere, Category = "HISM Placement")
    FRotator NextRotation;
};
