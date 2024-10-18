// Fill out your copyright notice in the Description page of Project Settings.

#include "Network/GPGameInstance.h"
#include "GPNetworkThread.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Character/GPCharacterPlayer.h"
#include "../../GP_Server/Proto.h"

void UGPGameInstance::Init()
{
	Super::Init();
	ConnectToServer();
}

void UGPGameInstance::Shutdown()
{
	DisconnectFromServer();
	Super::Shutdown();
}

void UGPGameInstance::ConnectToServer()
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
		NetworkThread = MakeShared<GPNetworkThread>(Socket);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void UGPGameInstance::DisconnectFromServer()
{
	if (NetworkThread)
	{
		NetworkThread->Destroy();
	}

	if (Socket)
	{
		this->SendPlayerLogoutPacket();
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
}

void UGPGameInstance::SendPlayerLoginPacket()
{
	FLoginPacket Packet;
	Packet.Header.PacketType = EPacketType::C_LOGIN;
	Packet.Header.PacketSize = sizeof(FLoginPacket);

	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FLoginPacket), BytesSent);
}

void UGPGameInstance::SendPlayerLogoutPacket()
{
	FLogoutPacket Packet;
	Packet.Header.PacketType = EPacketType::C_LOGOUT;
	Packet.Header.PacketSize = sizeof(FLogoutPacket);
	Packet.PlayerID = MyPlayer->PlayerInfo.ID;

	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FLogoutPacket), BytesSent);
}

void UGPGameInstance::SendPlayerMovePacket()
{
	FMovePacket Packet;
	Packet.Header.PacketType = EPacketType::C_MOVE;
	Packet.Header.PacketSize = sizeof(FMovePacket);
	Packet.PlayerInfo = MyPlayer->PlayerInfo;
	int32 BytesSent = 0;
	UE_LOG(LogTemp, Warning, TEXT("Send [%d] (%f,%f,%f)"), 
		Packet.PlayerInfo.ID, Packet.PlayerInfo.X, Packet.PlayerInfo.Y, Packet.PlayerInfo.Z);

	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FMovePacket), BytesSent);
}

void UGPGameInstance::ProcessPacket()
{
	TArray<uint8> PacketData;

	while (RecvQueue.Dequeue(PacketData))
	{
		if (PacketData.Num() >= sizeof(FPacketHeader))
		{
			FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(PacketData.GetData());

			switch (PacketHeader->PacketType)
			{
			case EPacketType::S_LOGININFO:
			{
				FLoginInfoPacket* LoginInfoPacket = reinterpret_cast<FLoginInfoPacket*>(PacketData.GetData());
				UE_LOG(LogTemp, Warning, TEXT("LOGININFO [%d] (%f,%f,%f)"),
					LoginInfoPacket->PlayerInfo.ID, 
					LoginInfoPacket->PlayerInfo.X,
					LoginInfoPacket->PlayerInfo.Y,
					LoginInfoPacket->PlayerInfo.Z);
				AddPlayer(LoginInfoPacket->PlayerInfo, true);
				break;
			}
			case EPacketType::S_ADD_PLAYER:
			{
				FAddPlayerPacket* AddPlayerPacket = reinterpret_cast<FAddPlayerPacket*>(PacketData.GetData());
				UE_LOG(LogTemp, Warning, TEXT("add [%d] (%f,%f,%f)"),
					AddPlayerPacket->PlayerInfo.ID,
					AddPlayerPacket->PlayerInfo.X,
					AddPlayerPacket->PlayerInfo.Y,
					AddPlayerPacket->PlayerInfo.Z);
				AddPlayer(AddPlayerPacket->PlayerInfo, false);
				break;
			}
			case EPacketType::S_REMOVE_PLAYER:
			{
				FLogoutPacket* RemovePlayerPacket = reinterpret_cast<FLogoutPacket*>(PacketData.GetData());
				break;
			}
			case EPacketType::S_MOVE_PLAYER:
			{
				FMovePacket* MovePlayerPacket = reinterpret_cast<FMovePacket*>(PacketData.GetData());
				UE_LOG(LogTemp, Warning, TEXT("other player move [%d] (%f,%f,%f)"),
					MovePlayerPacket->PlayerInfo.ID,
					MovePlayerPacket->PlayerInfo.X,
					MovePlayerPacket->PlayerInfo.Y,
					MovePlayerPacket->PlayerInfo.Z);
				break;
			}
			default:
				UE_LOG(LogTemp, Warning, TEXT("Unknown Packet Type received."));
				break;
			}
		}
	}
}


void UGPGameInstance::AddPlayer(FPlayerInfo& PlayerInfo, bool isMyPlayer)
{
	auto* World = GetWorld();
	if (World == nullptr)
		return;

	FVector SpawnLocation(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z);
	FRotator SpawnRotation(0, PlayerInfo.Yaw,0);

	if (isMyPlayer)
	{
		if (MyPlayer == nullptr)
			return;

		MyPlayer->SetPlayerInfo(PlayerInfo);
		Players.Add(PlayerInfo.ID, MyPlayer);
	}
	else
	{
		AGPCharacterBase* Player = nullptr;
		while (Player == nullptr)
		{
			Player = World->SpawnActor<AGPCharacterBase>(OtherPlayerClass, SpawnLocation, SpawnRotation);
		}
		Player->SetPlayerInfo(PlayerInfo);
		Players.Add(PlayerInfo.ID, Player);
	}
}

void UGPGameInstance::RemovePlayer(int32 ID)
{

}
