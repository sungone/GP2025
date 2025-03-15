#include "Network/GPGameInstance.h"
#include "Network/GPNetworkManager.h"
#include "Kismet/GameplayStatics.h"
#include "GPUtils.h"

void UGPGameInstance::Init()
{
	Super::Init();
	NetworkMgr = GetSubsystem<UGPNetworkManager>();
	NetworkMgr->ConnectToServer();
	ExportLevelBoundingBoxData(GetWorld());
}

void UGPGameInstance::Shutdown()
{
	NetworkMgr->DisconnectFromServer();
	Super::Shutdown();
}

void UGPGameInstance::ProcessPacket()
{
	NetworkMgr->ProcessPacket();
}
