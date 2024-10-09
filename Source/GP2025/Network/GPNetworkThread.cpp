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
			TArray<uint8> RData;
			RData.SetNumUninitialized(FMath::Min(DataSize, 65507u));

			int32 BytesRead = 0;
			Socket->Recv(RData.GetData(), RData.Num(), BytesRead);

			if (BytesRead > 0)
			{
				Buffer.Append(RData.GetData(), BytesRead);

				if (Buffer.Num() >= sizeof(FPacketHeader))
				{
					FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(Buffer.GetData());

					if (Buffer.Num() < PacketHeader->PacketSize)
						continue;

					switch (PacketHeader->PacketType)
					{
					case EPacketType::S_LOGININFO:
					{
						FLoginInfoPacket* LoginInfoPacket = reinterpret_cast<FLoginInfoPacket*>(Buffer.GetData());
						UE_LOG(LogTemp, Warning, TEXT("Login Info Packet received. PlayerID: %d"), LoginInfoPacket->PlayerID);
						break;
					}
					case EPacketType::S_ADD_PLAYER:
					{
						FAddPlayerPacket* AddPlayerPacket = reinterpret_cast<FAddPlayerPacket*>(Buffer.GetData());
						UE_LOG(LogTemp, Warning, TEXT("Add Player Packet received. PlayerID: %d, X: %f, Y: %f, Z: %f"),
							AddPlayerPacket->PlayerID, AddPlayerPacket->VecInfo.X, AddPlayerPacket->VecInfo.Y, AddPlayerPacket->VecInfo.Z);
						break;
					}
					case EPacketType::S_REMOVE_PLAYER:
					{
						UE_LOG(LogTemp, Warning, TEXT("Remove Player Packet received."));
						break;
					}
					case EPacketType::S_MOVE_PLAYER:
					{
						FMovePacket* MovePlayerPacket = reinterpret_cast<FMovePacket*>(Buffer.GetData());
						UE_LOG(LogTemp, Warning, TEXT("Move Player Packet received. PlayerID: %d, X: %f, Y: %f, Z: %f"),
							MovePlayerPacket->PlayerID, MovePlayerPacket->VecInfo.X, MovePlayerPacket->VecInfo.Y, MovePlayerPacket->VecInfo.Z);
						break;
					}
					default:
						UE_LOG(LogTemp, Warning, TEXT("Unknown Packet Type received."));
						break;
					}

					Buffer.RemoveAt(0, PacketHeader->PacketSize, false);
				}
			}
		}

		FPlatformProcess::Sleep(0.01f);
	}
	return 0;
}
