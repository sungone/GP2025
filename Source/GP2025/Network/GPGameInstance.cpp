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

//AABB박스 추출 -> TUK level BP에서 호출하고 있음
void UGPGameInstance::SaveBoundingBoxData(ULevel* Level)
{
	ExportLevelBoundingBoxData(Level, TEXT("GP_Server/BoundingBoxData.json"));
}

//NavMesh 추출 -> BP_MyPlayer에서 호출하고 있음(World생성 후 호출)
void UGPGameInstance::SaveNavData()
{
	ExtractNavMeshData(GetWorld(), TEXT("GP_Server/NavMeshData.json"));
}