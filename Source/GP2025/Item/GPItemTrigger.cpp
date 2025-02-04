// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GPItemTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/GPCollision.h"
#include "Interface/GPCharacterItemInterface.h"

// Sets default values
AGPItemTrigger::AGPItemTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemTrigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
	Effect->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_GPTRIGGER);
	Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AGPItemTrigger::OnOverlapBegin);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/StaticMesh/SM_ItemTriggerMesh.SM_ItemTriggerMesh'"));
	if (BoxMeshRef.Object)
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}

	Mesh->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT(""));
	//if (EffectRef.Object)
	//{
	//	Effect->SetTemplate(EffectRef.Object);
	//	Effect->bAutoActivate = false;
	//}
}

void AGPItemTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	if (nullptr == Item)
	{
		Destroy();
		return;
	}

	IGPCharacterItemInterface* OverlappingPawn = Cast<IGPCharacterItemInterface>(OtherActor);
	if (OverlappingPawn)
	{
		OverlappingPawn->TakeItem(Item);
	}
	// Effect->Activate(true);
	Mesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	// Effect->OnSystemFinished.AddDynamic(this, &AGPItemTrigger::OnEffectFinished);
}

void AGPItemTrigger::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	Destroy();
}
