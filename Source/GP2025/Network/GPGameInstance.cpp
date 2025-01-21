// Fill out your copyright notice in the Description page of Project Settings.

#include "Network/GPGameInstance.h"

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
		this->SendPlayerLogoutPacket();
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
}

void UGPGameInstance::SendPlayerLoginPacket()
{
	Packet Packet(EPacketType::C_LOGIN);
	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(Packet), BytesSent);
}

void UGPGameInstance::SendPlayerLogoutPacket()
{
	IDPacket Packet(EPacketType::C_LOGOUT, MyPlayer->PlayerInfo.ID);
	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(IDPacket), BytesSent);
}

void UGPGameInstance::SendPlayerMovePacket()
{
	InfoPacket Packet(EPacketType::C_MOVE, MyPlayer->PlayerInfo);
	int32 BytesSent = 0;
	UE_LOG(LogTemp, Warning, TEXT("SendPlayerMovePacket : Send [%d] (%f,%f,%f)"),
		Packet.Data.ID, Packet.Data.X, Packet.Data.Y, Packet.Data.Z);
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(InfoPacket), BytesSent);
}

void UGPGameInstance::sendPlayerAttackPacket()
{
	InfoPacket Packet(EPacketType::C_ATTACK, MyPlayer->PlayerInfo);
	int32 BytesSent = 0;
	UE_LOG(LogTemp, Log, TEXT("sendPlayerAttackPacket : Send [%d] (%f,%f,%f)"),
		Packet.Data.ID, Packet.Data.X, Packet.Data.Y, Packet.Data.Z);

	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(InfoPacket), BytesSent);
}

void UGPGameInstance::ReceiveData()
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

void UGPGameInstance::ProcessPacket()
{
	ReceiveData();

	TArray<uint8> PacketData;

	while (RecvQueue.Dequeue(PacketData))
	{
		RemainingData.Append(PacketData);

		while (RemainingData.Num() > sizeof(FPacketHeader))
		{
			FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(RemainingData.GetData());

			if (RemainingData.Num() >= PacketHeader->PacketSize)
			{
				switch (PacketHeader->PacketType)
				{
				case EPacketType::S_LOGININFO:
				{
					InfoPacket* LoginInfoPacket = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
					AddPlayer(LoginInfoPacket->Data, true);
					break;
				}
				case EPacketType::S_ADD_PLAYER:
				{
					InfoPacket* AddPlayerPacket = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
					AddPlayer(AddPlayerPacket->Data, false);
					break;
				}
				case EPacketType::S_REMOVE_PLAYER:
				{
					IDPacket* RemovePlayerPacket = reinterpret_cast<IDPacket*>(RemainingData.GetData());
					RemovePlayer(RemovePlayerPacket->Data);
					break;
				}
				case EPacketType::S_MOVE_PLAYER:
				{
					InfoPacket* MovePlayerPacket = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
					UpdatePlayer(MovePlayerPacket->Data);
					break;
				}
				case EPacketType::S_ATTACK_PLAYER :
				{
					InfoPacket* AttackPlayerPacket = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
					UpdatePlayer(AttackPlayerPacket->Data);
					break;
				}
				default:
					UE_LOG(LogTemp, Warning, TEXT("Unknown Packet Type received."));
					break;
				}

				RemainingData.RemoveAt(0, PacketHeader->PacketSize, false);
			}
			else
			{
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
	FRotator SpawnRotation(0, PlayerInfo.Yaw, 0);

	if (isMyPlayer)
	{
		if (MyPlayer == nullptr)
			return;

		UE_LOG(LogTemp, Warning, TEXT("Add my player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z, PlayerInfo.Yaw);

		MyPlayer->SetClientInfoFromServer(PlayerInfo);
		MyPlayer->SetActorLocation(FVector(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z));
		
		Players.Add(PlayerInfo.ID, MyPlayer);
	}
	else
	{
		AGPCharacterBase* Player = nullptr;
		while (Player == nullptr)
		{
			Player = World->SpawnActor<AGPCharacterBase>(OtherPlayerClass, SpawnLocation, SpawnRotation);
		}

		UE_LOG(LogTemp, Warning, TEXT("Add other player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z, PlayerInfo.Yaw);

		Player->SetClientInfoFromServer(PlayerInfo);
		Player->SetActorLocation(FVector(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z));
		
		Players.Add(PlayerInfo.ID, Player);
	}
}

void UGPGameInstance::RemovePlayer(int32 PlayerID)
{
	auto Player = Players.Find(PlayerID);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove player [%d]"), PlayerID);

		Players.Remove(PlayerID);
		(*Player)->Destroy();
	}
}

void UGPGameInstance::UpdatePlayer(FPlayerInfo& PlayerInfo)
{
	auto Player = Players.Find(PlayerInfo.ID);
	if (Player)
	{
		(*Player)->SetClientInfoFromServer(PlayerInfo);
		UE_LOG(LogTemp, Warning, TEXT("Update other player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z, PlayerInfo.Yaw);
	}
}
