// Fill out your copyright notice in the Description page of Project Settings.


#include "Sequence/GPSequenceManager.h"
#include "Kismet/GameplayStatics.h"

void UGPSequenceManager::PlaySequenceByName(UObject* WorldContext, FName SequenceName)
{
	if (!WorldContext)
	{
		UE_LOG(LogTemp, Error, TEXT("[SequenceManager] Invalid WorldContext"));
		return;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[SequenceManager] Invalid World"));
		return;
	}

	ULevelSequence* FoundSequence = nullptr;

	for (const FNamedSequence& Pair : SequenceList)
	{
		if (Pair.SequenceName == SequenceName)
		{
			FoundSequence = Pair.SequenceAsset;
			break;
		}
	}

	if (!FoundSequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SequenceManager] Sequence '%s' not found"), *SequenceName.ToString());
		return;
	}

	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = true;

	SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		World, FoundSequence, Settings, SequenceActor);

	if (SequencePlayer)
	{
		SequencePlayer->OnFinished.AddDynamic(this, &UGPSequenceManager::OnSequenceFinished);
		SequencePlayer->Play();
		UE_LOG(LogTemp, Log, TEXT("[SequenceManager] Playing sequence: %s"), *SequenceName.ToString());
	}
}

void UGPSequenceManager::SkipSequence()
{
	if (SequencePlayer && SequencePlayer->IsPlaying())
	{
		SequencePlayer->Stop();
		UE_LOG(LogTemp, Log, TEXT("[SequenceManager] Sequence skipped"));
	}
}

bool UGPSequenceManager::IsSequencePlaying() const
{
	return SequencePlayer && SequencePlayer->IsPlaying();
}

void UGPSequenceManager::OnSequenceFinished()
{
	UE_LOG(LogTemp, Log, TEXT("Sequence finished"));

	if (OnSequenceFinishedDelegate.IsBound())
	{
		OnSequenceFinishedDelegate.Execute();
		OnSequenceFinishedDelegate.Unbind(); // 한번만 호출되게
	}
}