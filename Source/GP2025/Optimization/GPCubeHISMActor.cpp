// Fill out your copyright notice in the Description page of Project Settings.


#include "Optimization/GPCubeHISMActor.h"

// Sets default values
AGPCubeHISMActor::AGPCubeHISMActor()
{
    PrimaryActorTick.bCanEverTick = false;

    HISMComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISMComponent"));
    RootComponent = HISMComponent;

    MeshToUse = nullptr;
    StartLocation = FVector::ZeroVector;
    NextLocation = FVector(100.f, 0.f, 0.f);
    InstanceCount = 10;
    InstanceScale = FVector(1.f, 1.f, 1.f);

}

// Called when the game starts or when spawned
void AGPCubeHISMActor::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AGPCubeHISMActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGPCubeHISMActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    HISMComponent->ClearInstances();

    if (!MeshToUse || InstanceCount < 1)
    {
        return;
    }

    HISMComponent->SetStaticMesh(MeshToUse);

    FVector Offset = NextLocation - StartLocation;

    for (int32 i = 0; i < InstanceCount; i++)
    {
        FVector Location = StartLocation + Offset * i;

        FTransform InstanceTransform;
        InstanceTransform.SetLocation(Location);
        InstanceTransform.SetRotation(FQuat::Identity);
        InstanceTransform.SetScale3D(InstanceScale);

        HISMComponent->AddInstance(InstanceTransform);
    }
}

