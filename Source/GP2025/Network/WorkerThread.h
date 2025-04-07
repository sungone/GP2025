#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"

class UGPNetworkManager;
class FWorkerThread : public FRunnable
{
public:
	FWorkerThread(UGPNetworkManager* InManager);
	virtual ~FWorkerThread() override;

	virtual uint32 Run() override;
	virtual void Stop() override;

private:
	FThreadSafeBool bStopThread;
	UGPNetworkManager* NetworkManager;
};
