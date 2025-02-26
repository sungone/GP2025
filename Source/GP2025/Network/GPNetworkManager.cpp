// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/GPNetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Network/GPObjectManager.h"
#include "Character/GPCharacterPlayer.h"

void UGPNetworkManager::ConnectToServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("ClientSocket"));
	Socket->SetNonBlocking(true);

	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Connecting To Server...")));

	Socket->Connect(*InternetAddr);

	if (Socket->GetConnectionState() == SCS_Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Connection Success")));
		SendPlayerLoginPacket();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void UGPNetworkManager::DisconnectFromServer()
{
	if (Socket)
	{
		this->SendPlayerLogoutPacket();
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
}

void UGPNetworkManager::SetMyPlayer(AGPCharacterPlayer* InMyPlayer)
{
	MyPlayer = InMyPlayer;
	UGPObjectManager* ObjectMgr = GetWorld()->GetSubsystem<UGPObjectManager>();
	ObjectMgr->SetMyPlayer(InMyPlayer);
}

void UGPNetworkManager::SendPlayerLoginPacket()
{
	Packet Packet(EPacketType::C_LOGIN);
	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(Packet), BytesSent);
}

void UGPNetworkManager::SendPlayerLogoutPacket()
{
	IDPacket Packet(EPacketType::C_LOGOUT, MyPlayer->CharacterInfo.ID);
	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(IDPacket), BytesSent);
}

void UGPNetworkManager::SendPlayerMovePacket()
{
	InfoPacket Packet(EPacketType::C_MOVE, MyPlayer->CharacterInfo);
	int32 BytesSent = 0;
	UE_LOG(LogTemp, Warning, TEXT("SendPlayerMovePacket : Send [%d]"), Packet.Data.ID);
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(InfoPacket), BytesSent);
}

void UGPNetworkManager::SendPlayerAttackPacket(int32 TargetID)
{
	AttackPacket Packet(TargetID);
	int32 BytesSent = 0;
	UE_LOG(LogTemp, Warning, TEXT("SendPlayerAttackPacket : Send [%d]"), MyPlayer->CharacterInfo.ID);
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(AttackPacket), BytesSent);
}

void UGPNetworkManager::SendPlayerTakeItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_TAKE_ITEM, ItemID);
	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(IDPacket), BytesSent);
}

void UGPNetworkManager::ReceiveData()
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
			RecvQueue.Enqueue(RecvData);
		}
	}
}

void UGPNetworkManager::ProcessPacket()
{
	ReceiveData();
	TArray<uint8> PacketData;
	UGPObjectManager* ObjectMgr = GetWorld()->GetSubsystem<UGPObjectManager>();
	while (RecvQueue.Dequeue(PacketData))
	{
		RemainingData.Append(PacketData);

		while (RemainingData.Num() > sizeof(FPacketHeader))
		{
			FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(RemainingData.GetData());

			if (RemainingData.Num() < PacketHeader->PacketSize) break;

			switch (PacketHeader->PacketType)
			{
			case EPacketType::S_LOGIN_SUCCESS:
			{
				InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
				ObjectMgr->AddPlayer(Pkt->Data, true);
				break;
			}
			case EPacketType::S_ADD_PLAYER:
			{
				InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
				ObjectMgr->AddPlayer(Pkt->Data, false);
				break;
			}
			case EPacketType::S_REMOVE_PLAYER:
			{
				IDPacket* Pkt = reinterpret_cast<IDPacket*>(RemainingData.GetData());
				ObjectMgr->RemovePlayer(Pkt->Data);
				break;
			}
			case EPacketType::S_PLAYER_STATUS_UPDATE:
			{
				InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
				ObjectMgr->UpdatePlayer(Pkt->Data);
				break;
			}
			case EPacketType::S_ADD_MONSTER:
			{
				InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
				ObjectMgr->AddMonster(Pkt->Data);
				break;
			}
			case EPacketType::S_REMOVE_MONSTER:
			{
				IDPacket* Pkt = reinterpret_cast<IDPacket*>(RemainingData.GetData());
				ObjectMgr->RemoveMonster(Pkt->Data);
				break;
			}
			case EPacketType::S_MONSTER_STATUS_UPDATE:
			{
				InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
				ObjectMgr->UpdateMonster(Pkt->Data);
				break;
			}
			case EPacketType::S_DAMAGED_MONSTER:
			{
				DamagePacket* Pkt = reinterpret_cast<DamagePacket*>(RemainingData.GetData());
				ObjectMgr->DamagedMonster(Pkt->Target, Pkt->Damage);
				break;
			}
			case EPacketType::S_ITEM_SPAWN:
			{
				ItemSpawnPacket* Pkt = reinterpret_cast<ItemSpawnPacket*>(RemainingData.GetData());
				ObjectMgr->ItemSpawn(Pkt->ItemID, Pkt->ItemType, Pkt->Pos);
				break;
			}
			case EPacketType::S_ITEM_DESPAWN:
			{
				ItemDespawnPacket* Pkt = reinterpret_cast<ItemDespawnPacket*>(RemainingData.GetData());
				ObjectMgr->ItemDespawn(Pkt->ItemID);
				break;
			}
			case EPacketType::S_ADD_IVENTORY_ITEM:
			{
				AddInventoryItemPacket* Pkt = reinterpret_cast<AddInventoryItemPacket*>(RemainingData.GetData());
				ObjectMgr->AddInventoryItem(Pkt->ItemType, Pkt->Quantity);
				break;
			}
			case EPacketType::S_REMOVE_IVENTORY_ITEM:
			{
				RemoveInventoryItemPacket* Pkt = reinterpret_cast<RemoveInventoryItemPacket*>(RemainingData.GetData());
				ObjectMgr->RemoveInventoryItem(Pkt->ItemType, Pkt->Quantity);
				break;
			}
			default:
				UE_LOG(LogTemp, Warning, TEXT("Unknown Packet Type received."));
				break;
			}

			RemainingData.RemoveAt(0, PacketHeader->PacketSize, false);
		}
	}
}

