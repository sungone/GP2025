// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/GPNetworkThread.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"

#include "../../GP_Server/Proto.h"

GPNetworkThread::GPNetworkThread(FSocket* Socket)
	: Socket(Socket) 
{
	Thread = FRunnableThread::Create(this, TEXT("WorkerThread"));
}

uint32 GPNetworkThread::Run()
{
	while (isThreadRuning)
	{
		uint32 DataSize;
		if (Socket->HasPendingData(DataSize))
		{
			TArray<uint8> ReceivedData;
			ReceivedData.SetNumUninitialized(FMath::Min(DataSize, 65507u));

			int32 BytesRead = 0;
			Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead);

			if (BytesRead > 0)
			{
				if (BytesRead >= sizeof(FPacketHeader))
				{
					FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(ReceivedData.GetData());

					switch (PacketHeader->PacketType)
					{
					case EPacketType::C_LOGIN:
					{
						if (BytesRead >= sizeof(FLoginPacket))
						{
							FLoginPacket* LoginPacket = reinterpret_cast<FLoginPacket*>(ReceivedData.GetData());
							UE_LOG(LogTemp, Warning, TEXT("Login Packet received. PlayerID: %d"), LoginPacket->PlayerID);
						}
						break;
					}
					case EPacketType::C_LOGINOUT:
					{
						UE_LOG(LogTemp, Warning, TEXT("Logout Packet received."));
						break;
					}
					case EPacketType::C_MOVE:
					{
						if (BytesRead >= sizeof(FMovePacket))
						{
							FMovePacket* MovePacket = reinterpret_cast<FMovePacket*>(ReceivedData.GetData());
							UE_LOG(LogTemp, Warning, TEXT("Move Packet received. PlayerID: %d, X: %f, Y: %f, Z: %f"),
								MovePacket->PlayerID, MovePacket->VecInfo.X, MovePacket->VecInfo.Y, MovePacket->VecInfo.Z);
						}
						break;
					}
					case EPacketType::S_LOGININFO:
					{
						if (BytesRead >= sizeof(FLoginInfoPacket))
						{
							FLoginInfoPacket* LoginInfoPacket = reinterpret_cast<FLoginInfoPacket*>(ReceivedData.GetData());
							UE_LOG(LogTemp, Warning, TEXT("Login Info Packet received. PlayerID: %d, X: %f, Y: %f, Z: %f"),
								LoginInfoPacket->PlayerID, LoginInfoPacket->VecInfo.X, LoginInfoPacket->VecInfo.Y, LoginInfoPacket->VecInfo.Z);
						}
						break;
					}
					case EPacketType::S_ADD_PLAYER:
					{
						if (BytesRead >= sizeof(FAddPlayerPacket))
						{
							FAddPlayerPacket* AddPlayerPacket = reinterpret_cast<FAddPlayerPacket*>(ReceivedData.GetData());
							UE_LOG(LogTemp, Warning, TEXT("Add Player Packet received. PlayerID: %d, X: %f, Y: %f, Z: %f"),
								AddPlayerPacket->PlayerID, AddPlayerPacket->VecInfo.X, AddPlayerPacket->VecInfo.Y, AddPlayerPacket->VecInfo.Z);
						}
						break;
					}
					case EPacketType::S_REMOVE_PLAYER:
					{
						UE_LOG(LogTemp, Warning, TEXT("Remove Player Packet received."));
						break;
					}
					case EPacketType::S_MOVE_PLAYER:
					{
						if (BytesRead >= sizeof(FMovePacket))
						{
							FMovePacket* MovePlayerPacket = reinterpret_cast<FMovePacket*>(ReceivedData.GetData());
							UE_LOG(LogTemp, Warning, TEXT("Move Player Packet received. PlayerID: %d, X: %f, Y: %f, Z: %f"),
								MovePlayerPacket->PlayerID, MovePlayerPacket->VecInfo.X, MovePlayerPacket->VecInfo.Y, MovePlayerPacket->VecInfo.Z);
						}
						break;
					}
					default:
						UE_LOG(LogTemp, Warning, TEXT("Unknown Packet Type received."));
						break;
					}
				}
			}
		}
		
		FPlatformProcess::Sleep(0.01f);
	}
	return 0;
}
