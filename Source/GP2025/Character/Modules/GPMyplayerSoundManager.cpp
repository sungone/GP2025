// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerSoundManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterMyplayer.h"

#define LOAD_SFX(SoundVar, AssetName) \
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundVar##Obj(TEXT("/Game/Sound/SFX/" #AssetName "." #AssetName)); \
	if (SoundVar##Obj.Succeeded()) SoundVar = SoundVar##Obj.Object;

UGPMyplayerSoundManager::UGPMyplayerSoundManager()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> LoginSoundAsset(TEXT("/Game/Sound/BackgroundBGM/LoginLobbySound.LoginLobbySound"));
	if (LoginSoundAsset.Succeeded())
	{
		LoginSound = LoginSoundAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> LobbySoundAsset(TEXT("/Game/Sound/BackgroundBGM/LoginLobbySound.LoginLobbySound"));
	if (LobbySoundAsset.Succeeded())
	{
		LobbySound = LobbySoundAsset.Object;
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

	LOAD_SFX(ClickSound, ClickSound);
	LOAD_SFX(GunnerAttackSound, GunnerAttackSound);
	LOAD_SFX(GunnerESkillSound, GunnerESkillSound);
	LOAD_SFX(GunnerRSkillSound, GunnerRSkillSound);
	LOAD_SFX(GunnerPlayerZoomSound, GunnerPlayerZoomSound);
	LOAD_SFX(GunnerQSkillSound, GunnerQSkillSound);
	LOAD_SFX(LevelUpSound, LevelUpSound);
	LOAD_SFX(MonsterHitSound, MonsterHitSound);
	LOAD_SFX(PlayerJumpSound, PlayerJumpSound);
	LOAD_SFX(PlayerPunchSound, PlayerPunchSound);
	LOAD_SFX(PlayerWalkSound, PlayerWalkSound);
	LOAD_SFX(QuestClearSound, QuestClearSound);
	LOAD_SFX(WarriorAttackSound, WarriorAttackSound);
	LOAD_SFX(WarriorESkillSound, WarriorESkillSound);
	LOAD_SFX(WarriorQSkillSound, WarriorQSkillSound);
	LOAD_SFX(WarriorRSkillSound, WarriorRSkillSound);
	LOAD_SFX(TeleportationSound, TeleportationSound);
	LOAD_SFX(WarningSound, WarningSound);
	LOAD_SFX(PickUpItemSound, PickUpItemSound);
	LOAD_SFX(PlayerHit, PlayerHit);
	LOAD_SFX(ShopBuySellSound, ShopBuySellSound);
	LOAD_SFX(FinalQuestClearSound, FinalQuestClearSound);
	LOAD_SFX(WarriorDeadSound, WarriorDeadSound);
	LOAD_SFX(GunnerDeadSound, GunnerDeadSound);

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
		if (BGMComponent->IsPlaying())
		{
			BGMComponent->Stop();
		}
	}
}

void UGPMyplayerSoundManager::PlayLoginBGM()
{
	PlayBGM(LoginSound , 1.f , true);
}

void UGPMyplayerSoundManager::StopLoginBGM()
{
	if (BGMComponent && BGMComponent->Sound == LoginSound)
	{
		BGMComponent->OnAudioFinished.Clear();
		BGMComponent->Stop();
	}
}

void UGPMyplayerSoundManager::PlayLobbyBGM()
{
	PlayBGM(LobbySound, 1.0f, true);
}

void UGPMyplayerSoundManager::StopLobbyBGM()
{
	if (BGMComponent && BGMComponent->Sound == LobbySound)
	{
		BGMComponent->OnAudioFinished.Clear(); 
		BGMComponent->Stop();
	}
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

void UGPMyplayerSoundManager::SetBGMVolume(float NewVolume)
{
	if (BGMComponent)
	{
		BGMComponent->SetVolumeMultiplier(FMath::Clamp(NewVolume, 0.f, 2.f));
	}
}

void UGPMyplayerSoundManager::PlaySFX(USoundBase* Sound, float Pitch, float Volume)
{
	if (!Sound)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundManager] PlaySFX failed: Sound is null"));
		return;
	}

	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SoundManager] PlaySFX failed: Owner is null"));
		return;
	}

	UGameplayStatics::PlaySound2D(Owner->GetWorld(), Sound, Volume, Pitch);

	UE_LOG(LogTemp, Log, TEXT("[SoundManager] PlaySFX: Playing '%s' at Volume %.2f, Pitch %.2f"), *Sound->GetName(), Volume, Pitch);
}

void UGPMyplayerSoundManager::HandleLoopBGM()
{
	if (BGMComponent && BGMComponent->Sound)
	{
		BGMComponent->Play(); // 다시 재생
	}
}
