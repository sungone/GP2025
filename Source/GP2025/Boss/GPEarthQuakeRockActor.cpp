// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/GPEarthQuakeRockActor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AGPEarthQuakeRockActor::AGPEarthQuakeRockActor()
{
	PrimaryActorTick.bCanEverTick = false; 

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RockMeshAsset(TEXT("/Game/Skill/Rock_earthquake.Rock_earthquake"));
	if (RockMeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(RockMeshAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load Rock StaticMesh"));
	}

	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGPEarthQuakeRockActor::BeginPlay()
{
	Super::BeginPlay();

}

void AGPEarthQuakeRockActor::Init(const FVector& TargetLocation)
{
	TargetPos = TargetLocation;
	TargetPos.Z = 0.f;

	StartPos = TargetPos + FVector(0.f, 0.f, 800.f); // ������ ����
	SetActorLocation(StartPos);

	ElapsedTime = 0.f;

	// 0.5�� ���� 20ȸ ������ �̵�
	GetWorldTimerManager().SetTimer(
		FallTimerHandle,
		this,
		&AGPEarthQuakeRockActor::UpdateFall,
		FallDuration / NumSteps,
		true
	);
}

void AGPEarthQuakeRockActor::UpdateFall()
{
	ElapsedTime += FallDuration / NumSteps;
	float Alpha = FMath::Clamp(ElapsedTime / FallDuration, 0.f, 1.f);
	FVector NewPos = FMath::Lerp(StartPos, TargetPos, Alpha);
	SetActorLocation(NewPos);

	if (Alpha >= 1.f)
	{
		GetWorldTimerManager().ClearTimer(FallTimerHandle);

		// TODO: �� �������� �浹 ó�� �Ǵ� ������ �̺�Ʈ ����

		// 0.5�� �� �ı�
		GetWorldTimerManager().SetTimer(
			DestroyTimerHandle,
			this,
			&AGPEarthQuakeRockActor::DelayedDestroy,
			0.5f,
			false
		);
	}
}

void AGPEarthQuakeRockActor::DelayedDestroy()
{
	Destroy();
}