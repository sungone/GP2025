#include "Network/GPGameInstance.h"
#include "Network/GPNetworkManager.h"
#include "Kismet/GameplayStatics.h"
#include "GPUtils.h"

void UGPGameInstance::Init()
{
	Super::Init();
	NetworkMgr = GetSubsystem<UGPNetworkManager>();
	NetworkMgr->ConnectToServer();
	NetworkMgr->SendPlayerLoginPacket("qwer", "1234", false);
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

//AABB박스 추출 -> TUK level BP에서 호출하고 있음
void UGPGameInstance::SaveBoundingBoxData(ULevel* Level)
{
	ExportLevelBoundingBoxData(Level, TEXT("GP_Server/BoundingBoxData.json"));
}

//NavMesh 추출
void UGPGameInstance::SaveNavData(bool IsSave)
{
	if (IsSave)
		ExtractNavMeshData(GetWorld(), TEXT("GP_Server/NavMeshData.json"));
}