// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/GPEarthQuakeRockActor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AGPEarthQuakeRockActor::AGPEarthQuakeRockActor()
{
	PrimaryActorTick.bCanEverTick = false; // Tick 사용 안 함

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RockMeshAsset(TEXT("/Game/Skill/Rock_shopk_Raw.Rock_shopk_Raw"));
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

	StartPos = GetActorLocation();
	ElapsedTime = 0.f;

	GetWorldTimerManager().SetTimer(
		FallTimerHandle,
		this,
		&AGPEarthQuakeRockActor::UpdateFall,
		FallDuration / NumSteps, // 예: 0.5초 / 20 = 0.025초 간격
		true
	);
}

void AGPEarthQuakeRockActor::Init(const FVector& TargetLocation)
{
	TargetPos = TargetLocation;
	TargetPos.Z = 0.f;
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

		// 충돌 시 이펙트 / 사운드
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, TargetPos);
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, TargetPos);

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