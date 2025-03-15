#include "Network/GPGameInstance.h"
#include "Network/GPNetworkManager.h"
#include "Kismet/GameplayStatics.h"
#include "GPUtils.h"

void UGPGameInstance::Init()
{
	Super::Init();
	NetworkMgr = GetSubsystem<UGPNetworkManager>();
	NetworkMgr->ConnectToServer();
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

void UGPGameInstance::SaveMapData(ULevel* Level, const FString& PathName)
{
	ExportLevelBoundingBoxData(Level, PathName);
}
