#include "WorkerThread.h"
#include "Network/GPNetworkManager.h"

FWorkerThread::FWorkerThread(UGPNetworkManager* InManager)
	: bStopThread(false), NetworkManager(InManager)
{
}

FWorkerThread::~FWorkerThread()
{
}

uint32 FWorkerThread::Run()
{
	while (!bStopThread)
	{
		if (NetworkManager)
		{
			NetworkManager->ProcessPacket();
		}
		FPlatformProcess::Sleep(0.01f);
	}
	return 0;
}

void FWorkerThread::Stop()
{
	bStopThread = true;
}
