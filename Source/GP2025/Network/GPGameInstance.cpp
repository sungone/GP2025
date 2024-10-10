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
	Packet.PlayerID = NULL;

	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FLoginPacket), BytesSent);
}

void UGPGameInstance::SendPlayerLogoutPacket()
{
	FLogoutPacket Packet;
	Packet.Header.PacketType = EPacketType::C_LOGOUT;
	Packet.Header.PacketSize = sizeof(FLogoutPacket);
	Packet.PlayerID = this->PlayerID;

	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FLogoutPacket), BytesSent);
}

void UGPGameInstance::SendPlayerMovePacket(FVector Position, FRotator Rotation, bool IsJumping)
{
	FMovePacket Packet;
	Packet.Header.PacketType = EPacketType::C_MOVE;
	Packet.Header.PacketSize = sizeof(FMovePacket);
	Packet.PlayerID = this->PlayerID;
	Packet.VecInfo.X = Position.X;
	Packet.VecInfo.Y = Position.Y;
	Packet.VecInfo.Z = Position.Z;
	Packet.VecInfo.Yaw = Rotation.Yaw;
	Packet.VecInfo.Pitch = Rotation.Pitch;
	Packet.VecInfo.Roll = Rotation.Roll;
	Packet.IsJumping = IsJumping;

	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FMovePacket), BytesSent);
}

AGPCharacterPlayer* UGPGameInstance::FindPlayerByID(int32 ID)
{
	for (const TPair<AGPCharacterPlayer* , int32>& Pair : PlayerArr)
	{
		if (Pair.Value == ID)
		{
			return Pair.Key;
		}
	}

	return nullptr;
}

void UGPGameInstance::AddPlayer(AGPCharacterPlayer* NewPlayer , int32 ID)
{
	if (NewPlayer)	PlayerArr.Add(NewPlayer , ID);
}

void UGPGameInstance::RemovePlayer(int32 ID)
{
	AGPCharacterPlayer* Player = FindPlayerByID(ID);
	if (Player)
	{
		PlayerArr.Remove(Player);
	}
}
