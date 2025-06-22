// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerSoundManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterMyplayer.h"

UGPMyplayerSoundManager::UGPMyplayerSoundManager()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> LoginSoundAsset(TEXT("/Game/Sound/BGM/LoginSound.LoginSound"));
	if (LoginSoundAsset.Succeeded())
	{
		LoginSound = LoginSoundAsset.Object;
	}
}

void UGPMyplayerSoundManager::Initialize(AGPCharacterMyplayer* InOwner)
{
	Owner = InOwner;

	// BGMComponent �ʱ�ȭ
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
		// ���� ��������Ʈ ���ε� ����
		BGMComponent->OnAudioFinished.Clear();

		// ����� ������ �ٽ� ����
		BGMComponent->OnAudioFinished.AddDynamic(this, &UGPMyplayerSoundManager::HandleLoopBGM);
	}
	else
	{
		BGMComponent->OnAudioFinished.Clear();
	}
}

void UGPMyplayerSoundManager::HandleLoopBGM()
{
	if (BGMComponent && BGMComponent->Sound)
	{
		BGMComponent->Play(); // �ٽ� ���
	}
}

void UGPMyplayerSoundManager::PlayLoginBGM()
{
	PlayBGM(LoginSound);
}

void UGPMyplayerSoundManager::StopBGM()
{
	if (BGMComponent)
	{
		BGMComponent->Stop();
	}
}

void UGPMyplayerSoundManager::PlaySFX(USoundBase* Sound, float Volume)
{
	if (Sound && Owner)
	{
		UGameplayStatics::PlaySound2D(Owner->GetWorld(), Sound, Volume);
	}
}