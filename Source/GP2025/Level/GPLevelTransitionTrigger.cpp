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
#include "Engine/World.h"
#include "Engine/LevelStreaming.h"
#include "Engine/Engine.h"

AGPLevelTransitionTrigger::AGPLevelTransitionTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(200.f, 200.f, 200.f));

	TriggerBox->SetCollisionProfileName(CPROFILE_GPTRIGGER);
	TriggerBox->SetGenerateOverlapEvents(true);

	// 이펙트 생성
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

	FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &AGPLevelTransitionTrigger::OnLevelRemoved);
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &AGPLevelTransitionTrigger::OnLevelAdded);
}

void AGPLevelTransitionTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 콜백 해제
	FWorldDelegates::LevelRemovedFromWorld.RemoveAll(this);
	FWorldDelegates::LevelAddedToWorld.RemoveAll(this);
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
	if (!TriggerBox) return;

	CachedPlayer = Cast<AGPCharacterMyplayer>(OtherActor);
	if (!CachedPlayer || !IsValid(CachedPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedPlayer is invalid or pending kill in OnOverlapBegin."));
		return;
	}

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
			UE_LOG(LogTemp, Log, TEXT("[AGPLevelTransitionTrigger] SendMyZoneChangePacket Send Success"));

			if (NewZone == ZoneType::E)
			{
				NetworkMgr->SendMyCompleteQuest(QuestType::CH1_GO_TO_E_FIRST);
				UE_LOG(LogTemp, Log, TEXT("[AGPLevelTransitionTrigger] CH1_GO_TO_E_FIRST Quest Complete Packet Sent"));
			}

			// 퀘스트 E동으로 이동한다 : 보상 -> 문이 잠겨있어 경비아저씨를 찾는다 : 퀘스트 시작
			//if (NewZone == ZoneType::E)
			//{
			//	if (CachedPlayer)
			//	{
			//		QuestStatus CurQuest = CachedPlayer->CharacterInfo.CurrentQuest;
			//		if (CurQuest.QuestType == QuestType::CH1_GO_TO_E_FIRST
			//			&& CurQuest.Status == EQuestStatus::InProgress)
			//		{
			//			NetworkMgr->SendMyCompleteQuest(CurQuest.QuestType);
			//			UE_LOG(LogTemp, Log, TEXT("[AGPLevelTransitionTrigger] Correct Quest -> CH1_GO_TO_E_FIRST Complete Sent"));
			//		}
			//		else
			//		{
			//			UE_LOG(LogTemp, Warning, TEXT("[AGPLevelTransitionTrigger] Entered E zone, but quest mismatch or not active"));
			//		}
			//	}
			//}
		}
	}
}

void AGPLevelTransitionTrigger::OnLevelRemoved(ULevel* Level, UWorld* World)
{
	if (Level && World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Level %s removed from world %s"), *Level->GetName(), *World->GetName());
		CachedPlayer = nullptr;
	}
}

void AGPLevelTransitionTrigger::OnLevelAdded(ULevel* Level, UWorld* World)
{
	if (Level && World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Level %s added to world %s"), *Level->GetName(), *World->GetName());
	}
}
