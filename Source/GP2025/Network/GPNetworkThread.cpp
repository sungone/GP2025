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
						// 내 플레이어 아이디 받아옴
						// (위치는 나중에 마지막 접속 위치 불러와야 하게되면 사용하거나,
						//  로비 위치에 랜덤하게 위치 정보 받아올 수도 있고..)
						FLoginInfoPacket* LoginInfoPacket = reinterpret_cast<FLoginInfoPacket*>(Buffer.GetData());
						// -> 내플레이어 아이디 값 설정해주기.

						break;
					}
					case EPacketType::S_ADD_PLAYER:
					{
						// 다른 플레이어가 새로 접속하면 새로운 플레이어 정보 받아냄
						FAddPlayerPacket* AddPlayerPacket = reinterpret_cast<FAddPlayerPacket*>(Buffer.GetData());
						// -> 새로운 플레이어 추가해주기
						break;
					}
					case EPacketType::S_REMOVE_PLAYER:
					{
						// 다른 플레이어가 로그아웃하면 받음
						FLogoutPacket* RemovePlayerPacket = reinterpret_cast<FLogoutPacket*>(Buffer.GetData());
						// -> 로그아웃한 플레이어 제거해주기
						break;
					}
					case EPacketType::S_MOVE_PLAYER:
					{
						// 다른 플레이어가 움직이면 그 정보받음
						FMovePacket* MovePlayerPacket = reinterpret_cast<FMovePacket*>(Buffer.GetData());
						//-> 움직인 다른 플레이어의 값을 업데이트해주기

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
