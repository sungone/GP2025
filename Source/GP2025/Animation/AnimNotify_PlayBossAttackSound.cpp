// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_PlayBossAttackSound.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Actor.h"

void UAnimNotify_PlayBossAttackSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !SoundToPlay)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(Owner, SoundToPlay, Owner->GetActorLocation());
}
