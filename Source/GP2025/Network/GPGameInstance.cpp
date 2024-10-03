// Fill out your copyright notice in the Description page of Project Settings.

#include "Network/GPGameInstance.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "../../GP2025/GP_Server/Proto.h"

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
	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Connecting To Server...")));

	bool Connected = Socket->Connect(*InternetAddr);

	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Connection Success")));
		SendPlayerLoginPacket();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void UGPGameInstance::DisconnectFromServer()
{
	if (Socket)
	{
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

void UGPGameInstance::SendPlayerMovePacket(FVector Position, FRotator Rotation)
{
	FMovePacket Packet;
	Packet.Header.PacketType = EPacketType::C_MOVE;
	Packet.Header.PacketSize = sizeof(FMovePacket);
	Packet.PlayerID = PlayerID;
	Packet.X = Position.X;
	Packet.Y = Position.Y;
	Packet.Z = Position.Z;
	Packet.Yaw = Rotation.Yaw;
	Packet.Pitch = Rotation.Pitch;
	Packet.Roll = Rotation.Roll;

	int32 BytesSent = 0;
	if (Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FMovePacket), BytesSent))
	{
		UE_LOG(LogTemp, Log, TEXT("Sent Move Packet: PlayerID=%d, Position=(%f, %f, %f), Rotation=(Yaw: %f, Pitch: %f, Roll: %f)"),
			Packet.PlayerID, Packet.X, Packet.Y, Packet.Z, Packet.Yaw, Packet.Pitch, Packet.Roll);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to send Move Packet for PlayerID=%d"), Packet.PlayerID);
	}
}
