// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerSoundManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterMyplayer.h"

UGPMyplayerSoundManager::UGPMyplayerSoundManager()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> LoginSoundAsset(TEXT("/Game/Sound/BackgroundBGM/LoginLobbySound.LoginLobbySound"));
	if (LoginSoundAsset.Succeeded())
	{
		LoginSound = LoginSoundAsset.Object;
	}

	struct FSoundLoadInfo
	{
		FName LevelName;
		const TCHAR* AssetPath;
	};

	FSoundLoadInfo SoundInfos[] = {
		{ "TIP", TEXT("/Game/Sound/BackgroundBGM/TIPSound.TIPSound") },
		{ "TUWorld", TEXT("/Game/Sound/BackgroundBGM/OutsideSound.OutsideSound") },
		{ "TUK", TEXT("/Game/Sound/BackgroundBGM/OutsideSound.OutsideSound") },
		{ "E", TEXT("/Game/Sound/BackgroundBGM/ESound.ESound") },
		{ "Industry", TEXT("/Game/Sound/BackgroundBGM/IndustrySound.IndustrySound") },
		{ "Gym", TEXT("/Game/Sound/BackgroundBGM/GymSound.GymSound") }
	};

	for (const FSoundLoadInfo& Info : SoundInfos)
	{
		ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(Info.AssetPath);
		if (SoundObj.Succeeded())
		{
			LevelBGMSounds.Add(Info.LevelName, SoundObj.Object);
		}
	}
}

void UGPMyplayerSoundManager::Initialize(AGPCharacterMyplayer* InOwner)
{
	Owner = InOwner;

	if (Owner)
	{
		BGMComponent = NewObject<UAudioComponent>(Owner);
		if (BGMComponent)
		{
			BGMComponent->bAutoActivate = false;
			BGMComponent->bIsUISound = true;
			BGMComponent->RegisterComponentWithWorld(Owner->GetWorld());
		}
	}
}

void UGPMyplayerSoundManager::PlayBGM(USoundBase* Sound, float Volume, bool bLoop)
{
	if (!Sound || !BGMComponent) return;

	BGMComponent->Stop();
	BGMComponent->SetSound(Sound);
	BGMComponent->SetVolumeMultiplier(Volume);
	BGMComponent->Play();

	if (bLoop)
	{
		BGMComponent->OnAudioFinished.Clear();
		BGMComponent->OnAudioFinished.AddDynamic(this, &UGPMyplayerSoundManager::HandleLoopBGM);
	}
	else
	{
		BGMComponent->OnAudioFinished.Clear();
	}
}

void UGPMyplayerSoundManager::StopBGM()
{
	if (BGMComponent)
	{
		BGMComponent->Stop();
	}
}

void UGPMyplayerSoundManager::PlayLoginBGM()
{
	PlayBGM(LoginSound);
}

void UGPMyplayerSoundManager::PlayBGMForCurrentLevel()
{
	if (!Owner || !Owner->GetWorld()) return;

	FString FullMapName = Owner->GetWorld()->GetMapName();
	FString ShortMapName = FPackageName::GetShortName(FullMapName);

	// PIE 접두어 제거
	ShortMapName = ShortMapName.Replace(TEXT("UEDPIE_0_"), TEXT(""));

	FName LevelName(*ShortMapName);

	if (USoundBase** FoundSound = LevelBGMSounds.Find(LevelName))
	{
		if (*FoundSound)
		{
			UE_LOG(LogTemp, Log, TEXT("[SoundManager] Playing BGM for level: %s → %s"),
				*ShortMapName,
				*(*FoundSound)->GetName());

			PlayBGM(*FoundSound);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundManager] No BGM mapped for level: %s"), *ShortMapName);
	}
}

void UGPMyplayerSoundManager::PlayBGMByLevelName(const FName& LevelName)
{
	if (USoundBase** FoundSound = LevelBGMSounds.Find(LevelName))
	{
		PlayBGM(*FoundSound);
		UE_LOG(LogTemp, Log, TEXT("[SoundManager] Playing BGM for level: %s"), *LevelName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundManager] No BGM mapped for level: %s"), *LevelName.ToString());
	}
}

void UGPMyplayerSoundManager::PlaySFX(USoundBase* Sound, float Volume)
{
	if (Sound && Owner)
	{
		UGameplayStatics::PlaySound2D(Owner->GetWorld(), Sound, Volume);
	}
}

void UGPMyplayerSoundManager::HandleLoopBGM()
{
	if (BGMComponent && BGMComponent->Sound)
	{
		BGMComponent->Play(); // 다시 재생
	}
}
