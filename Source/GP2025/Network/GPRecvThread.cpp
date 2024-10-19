// Fill out your copyright notice in the Description page of Project Settings.

#include "Network/GPRecvThread.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Character/GPCharacterPlayer.h"
#include "Network/GPGameInstance.h"
#include "Engine/World.h"
#include "../../GP_Server/Proto.h"

GPRecvThread::GPRecvThread(FSocket* Socket)
	: Socket(Socket)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvThread"));
}

uint32 GPRecvThread::Run()
{
	while (!GWorld)
	{
		FPlatformProcess::Sleep(0.1f);
	}

	UGPGameInstance* GameInst = Cast<UGPGameInstance>(GWorld->GetGameInstance());
	if (!GameInst)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance cast failed"));
		return 1;
	}

	while (isThreadRuning)
	{
		uint32 DataSize;
		if (Socket->HasPendingData(DataSize))
		{
			TArray<uint8> RecvData;
			RecvData.SetNumUninitialized(FMath::Min(DataSize, 65507u));

			int32 BytesRead = 0;
			Socket->Recv(RecvData.GetData(), RecvData.Num(), BytesRead);

			if (BytesRead > 0)
			{
				GameInst->RecvQueue.Enqueue(RecvData);
			}
		}

		FPlatformProcess::Sleep(0.01f);
	}
	return 0;
}
