// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/GPLevelTransitionTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "Physics/GPCollision.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AGPLevelTransitionTrigger::AGPLevelTransitionTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(200.f, 200.f, 200.f));

	TriggerBox->SetCollisionProfileName(CPROFILE_GPTRIGGER);
	TriggerBox->SetGenerateOverlapEvents(true);


	// ÀÌÆåÆ® »ý¼º
	PortalEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
	PortalEffect->SetupAttachment(RootComponent);
	PortalEffect->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	PortalEffect->bAutoActivate = true;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PortalEffectAsset(TEXT("/Game/effect/ARPGEssentials/Effects/NS_ARPGEssentials_Loot_04_Epic.NS_ARPGEssentials_Loot_04_Epic"));
	if (PortalEffectAsset.Succeeded())
	{
		PortalEffect->SetAsset(PortalEffectAsset.Object);
	}

	PortalEffect->SetWorldScale3D(FVector(4.0f, 4.0f, 1.0f));
}

void AGPLevelTransitionTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &AGPLevelTransitionTrigger::OnOverlapBegin);
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGPLevelTransitionTrigger::OnOverlapBegin);
	}
}

void AGPLevelTransitionTrigger::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	CachedPlayer = Cast<AGPCharacterMyplayer>(OtherActor);
	if (!CachedPlayer) return;

	if (!LevelToLoad.IsNone())
	{
		UGPNetworkManager* NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
		if (NetworkMgr)
		{
			ZoneType NewZone = ZoneType::TUK;

			if (LevelToLoad == "tip")       NewZone = ZoneType::TIP;
			else if (LevelToLoad == "E")    NewZone = ZoneType::E;
			else if (LevelToLoad == "gym")  NewZone = ZoneType::GYM;
			else if (LevelToLoad == "TUK")  NewZone = ZoneType::TUK;
			else if (LevelToLoad == "industry")  NewZone = ZoneType::INDUSTY;

			NetworkMgr->SendMyZoneChangePacket(NewZone);
		}
	}
}
