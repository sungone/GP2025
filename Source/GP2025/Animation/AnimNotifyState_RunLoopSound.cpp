// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_RunLoopSound.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/Modules/GPMyplayerSoundManager.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyState_RunLoopSound::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    AccumulatedTime = 0.f;
    UE_LOG(LogTemp, Log, TEXT("[WalkLoopSound] Begin walking sound loop"));
}

void UAnimNotifyState_RunLoopSound::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    AccumulatedTime += FrameDeltaTime;

    if (AccumulatedTime >= 0.25f) // 0.25 초마다 소리
    {
        AActor* OwnerActor = MeshComp->GetOwner();
        if (AGPCharacterMyplayer* Player = Cast<AGPCharacterMyplayer>(OwnerActor))
        {
            if (Player->SoundManager && Player->SoundManager->PlayerWalkSound)
            {
                Player->SoundManager->PlaySFX(Player->SoundManager->PlayerWalkSound);
                UE_LOG(LogTemp, Log, TEXT("[RundLoopSound] Playing Run sound"));
            }
        }

        AccumulatedTime = 0.f;
    }
}

void UAnimNotifyState_RunLoopSound::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    UE_LOG(LogTemp, Log, TEXT("[WalkLoopSound] End walking sound loop"));
}

