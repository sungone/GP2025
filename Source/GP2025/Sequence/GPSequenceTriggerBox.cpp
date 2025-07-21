// Fill out your copyright notice in the Description page of Project Settings.


#include "Sequence/GPSequenceTriggerBox.h"
#include "Engine/World.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPObjectManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AGPSequenceTriggerBox::AGPSequenceTriggerBox()
{
	OnActorBeginOverlap.AddDynamic(this, &AGPSequenceTriggerBox::OnOverlapBegin);
}

void AGPSequenceTriggerBox::BeginPlay()
{
    Super::BeginPlay();
}

void AGPSequenceTriggerBox::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{

    if (!OtherActor) return;

    AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(OtherActor);
    if (MyPlayer)
    {
        if (MyPlayer->bHasPlayedTinoSequence)
        {
            UE_LOG(LogTemp, Log, TEXT("This player has already played the sequence."));
            return;
        }

        // 시퀀스 재생
        if (!LevelSequenceAsset)
        {
            UE_LOG(LogTemp, Warning, TEXT("[GPSequenceTriggerBox] LevelSequenceAsset is null."));
            return;
        }

        FMovieSceneSequencePlaybackSettings PlaybackSettings;
        PlaybackSettings.bAutoPlay = false;

        SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(),
            LevelSequenceAsset,
            PlaybackSettings,
            SequenceActor
        );

        if (SequencePlayer)
        {
            SequencePlayer->Play();
            UE_LOG(LogTemp, Log, TEXT("Sequence played for player %s"), *MyPlayer->GetName());
            MyPlayer->bHasPlayedTinoSequence = true;
        }
    }

    UGPObjectManager* ObjectMgr = GetWorld()->GetSubsystem<UGPObjectManager>();
    if (ObjectMgr)
    {
        // 티노 시퀀스 수행 후 티노 다시 Visible
        ObjectMgr->HideTinoMonstersTemporarily(6.f);
    }
}