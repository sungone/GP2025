#include "Network/GPGameInstance.h"
#include "Network/GPNetworkManager.h"
#include "Kismet/GameplayStatics.h"

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

void UGPGameInstance::SetMyPlayer(AGPCharacterPlayer* InMyPlayer)
{
	MyPlayer = InMyPlayer;
	NetworkMgr->SetMyPlayer(InMyPlayer);
}

void UGPGameInstance::SendPlayerLoginPacket()
{
	NetworkMgr->SendPlayerLoginPacket();
}

void UGPGameInstance::SendPlayerLogoutPacket()
{
	NetworkMgr->SendPlayerLogoutPacket();
}

void UGPGameInstance::SendPlayerMovePacket()
{
	NetworkMgr->SendPlayerMovePacket();
}

void UGPGameInstance::SendPlayerAttackPacket(int32 TargetID)
{
	NetworkMgr->SendPlayerAttackPacket(TargetID);
}

void UGPGameInstance::SendPlayerTakeItem(int32 ItemID)
{
	NetworkMgr->SendPlayerTakeItem(ItemID);
}

void UGPGameInstance::ProcessPacket()
{
	NetworkMgr->ProcessPacket();
}
