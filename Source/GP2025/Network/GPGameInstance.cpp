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

//AABB�ڽ� ���� -> TUK level BP���� ȣ���ϰ� ����
void UGPGameInstance::SaveBoundingBoxData(ULevel* Level)
{
	ExportLevelBoundingBoxData(Level, TEXT("GP_Server/BoundingBoxData.json"));
}

//NavMesh ����
void UGPGameInstance::SaveNavData(bool IsSave)
{
	if (IsSave)
		ExtractNavMeshData(GetWorld(), TEXT("GP_Server/NavMeshData.json"));
}