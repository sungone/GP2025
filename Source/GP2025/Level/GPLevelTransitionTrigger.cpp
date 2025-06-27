// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/GPLevelTransitionTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "Physics/GPCollision.h"
#include "Inventory/GPInventory.h"
#include "NiagaraComponent.h"
#include "Character/Modules/GPMyplayerSoundManager.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "NiagaraFunctionLibrary.h"
#include "UI/GPInGameWidget.h"
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
		if (!NetworkMgr) return;

		ZoneType NewZone = ZoneType::TUK;

		if (LevelToLoad == "tip")       NewZone = ZoneType::TIP;
		else if (LevelToLoad == "E")    NewZone = ZoneType::E;
		else if (LevelToLoad == "gym")  NewZone = ZoneType::GYM;
		else if (LevelToLoad == "TUK")  NewZone = ZoneType::TUK;
		else if (LevelToLoad == "industry")  NewZone = ZoneType::INDUSTY;

		// 이 부분이 핵심:
		if (NewZone == ZoneType::E)
		{
			// 1. 인벤토리 접근
			if (CachedPlayer->UIManager && CachedPlayer->UIManager->GetInventoryWidget())
			{
				UGPInventory* Inventory = CachedPlayer->UIManager->GetInventoryWidget();

				// 2. RowName 25번 아이템 보유 확인
				if (Inventory->HasItemByType(50)) // 열쇠 아이템이 있을 때에만 포탈 이동 가능
				{
					if (CachedPlayer->SoundManager && CachedPlayer->SoundManager->TeleportationSound)
					{
						CachedPlayer->SoundManager->PlaySFX(CachedPlayer->SoundManager->TeleportationSound);
					}

					// 아이템 있음 → 정상 입장 처리
					NetworkMgr->SendMyZoneChangePacket(NewZone);
					ShowZoneChangeMessage(NewZone);
					NetworkMgr->SendMyCompleteQuest(QuestType::CH2_ENTER_E_BUILDING);
					UE_LOG(LogTemp, Log, TEXT("[LevelTransitionTrigger] ZoneChange + QuestComplete Success"));
				}
				else
				{
					if (CachedPlayer->SoundManager && CachedPlayer->SoundManager->WarningSound)
					{
						CachedPlayer->SoundManager->PlaySFX(CachedPlayer->SoundManager->WarningSound);
					}

					UE_LOG(LogTemp, Warning, TEXT("[LevelTransitionTrigger] Item Type 25 not found. Cannot enter Zone E."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[LevelTransitionTrigger] Inventory not valid."));
			}
		}
		else
		{
			// E가 아닌 일반적인 존 이동

			if (CachedPlayer->SoundManager && CachedPlayer->SoundManager->TeleportationSound)
			{
				CachedPlayer->SoundManager->PlaySFX(CachedPlayer->SoundManager->TeleportationSound);
			}

			NetworkMgr->SendMyZoneChangePacket(NewZone);
			ShowZoneChangeMessage(NewZone);
			UE_LOG(LogTemp, Log, TEXT("[LevelTransitionTrigger] SendMyZoneChangePacket Send Success"));
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
		const FString RawLevelName = Level->GetOuter()->GetName();  // 정확한 레벨 이름
		UE_LOG(LogTemp, Warning, TEXT("Level %s added to world %s"), *RawLevelName, *World->GetName());

		if (CachedPlayer && CachedPlayer->SoundManager)
		{
			CachedPlayer->SoundManager->StopBGM();
			CachedPlayer->SoundManager->PlayBGMByLevelName(FName(*RawLevelName));
		}
	}
}

void AGPLevelTransitionTrigger::ShowZoneChangeMessage(ZoneType NewZone)
{
	if (CachedPlayer && CachedPlayer->UIManager && CachedPlayer->UIManager->GetInGameWidget())
	{
		FString ZoneNameString;

		switch (NewZone)
		{
		case ZoneType::TIP:
			ZoneNameString = TEXT("TIP");
			break;
		case ZoneType::E:
			ZoneNameString = TEXT("Building E");
			break;
		case ZoneType::GYM:
			ZoneNameString = TEXT("GYM");
			break;
		case ZoneType::TUK:
			ZoneNameString = TEXT("TUK");
			break;
		case ZoneType::INDUSTY:
			ZoneNameString = TEXT("Building Industry");
			break;
		default:
			ZoneNameString = TEXT("");
			break;
		}

		CachedPlayer->UIManager->GetInGameWidget()->ShowGameMessage(ZoneNameString, 3.0f);

		UE_LOG(LogTemp, Log, TEXT("[LevelTransitionTrigger] ZoneChange Message: %s"), *ZoneNameString);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[LevelTransitionTrigger] ShowZoneChangeMessage - CachedPlayer or UIManager is NULL"));
	}
}
