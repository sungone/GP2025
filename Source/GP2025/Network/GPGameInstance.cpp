// Fill out your copyright notice in the Description page of Project Settings.

#include "Network/GPGameInstance.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Character/GPCharacterPlayer.h"
#include "CharacterStat/GPCharacterStatComponent.h"
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
	Packet.PlayerID = MyPlayer->CharacterInfo.ID;

	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FLogoutPacket), BytesSent);
}

void UGPGameInstance::SendPlayerMovePacket()
{
	FMovePacket Packet;
	Packet.Header.PacketType = EPacketType::C_MOVE;
	Packet.Header.PacketSize = sizeof(FMovePacket);
	Packet.PlayerInfo = MyPlayer->CharacterInfo;

	int32 BytesSent = 0;

	UE_LOG(LogTemp, Warning, TEXT("SendPlayerMovePacket : Send [%d] (%f,%f,%f)"),
		Packet.PlayerInfo.ID, Packet.PlayerInfo.X, Packet.PlayerInfo.Y, Packet.PlayerInfo.Z);

	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FMovePacket), BytesSent);
}

void UGPGameInstance::SendPlayerAttackPacket()
{
	FAttackPacket Packet;
	Packet.Header.PacketType = EPacketType::C_ATTACK;
	Packet.Header.PacketSize = sizeof(FAttackPacket);
	Packet.PlayerInfo = MyPlayer->CharacterInfo;

	int32 BytesSent = 0;

	UE_LOG(LogTemp, Log, TEXT("sendPlayerAttackPacket : Send [%d] (%f,%f,%f)"),
		Packet.PlayerInfo.ID, Packet.PlayerInfo.X, Packet.PlayerInfo.Y, Packet.PlayerInfo.Z);

	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FAttackPacket), BytesSent);
}

void UGPGameInstance::SendHitPacket(FCharacterInfo& Attacker, FCharacterInfo& Attacked , bool isAttackerPlayer)
{
	FHitPacket Packet;
	Packet.Header.PacketType = EPacketType::C_HIT;
	Packet.Header.PacketSize = sizeof(FHitPacket);
	Packet.AttackerInfo = Attacker;
	Packet.attackedInfo = Attacked;
	Packet.isAttackerPlayer = isAttackerPlayer;

	int32 BytesSent = 0;

	UE_LOG(LogTemp, Log, TEXT("Attack %d -> %d"), Packet.AttackerInfo.ID , Packet.attackedInfo.ID);

	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(FHitPacket), BytesSent);
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
					FLoginInfoPacket* LoginInfoPacket = reinterpret_cast<FLoginInfoPacket*>(RemainingData.GetData());
					AddPlayer(LoginInfoPacket->PlayerInfo, true);
					break;
				}
				case EPacketType::S_ADD_PLAYER:
				{
					FAddPlayerPacket* AddPlayerPacket = reinterpret_cast<FAddPlayerPacket*>(RemainingData.GetData());
					AddPlayer(AddPlayerPacket->PlayerInfo, false);
					break;
				}
				case EPacketType::S_REMOVE_PLAYER:
				{
					FLogoutPacket* RemovePlayerPacket = reinterpret_cast<FLogoutPacket*>(RemainingData.GetData());
					RemovePlayer(RemovePlayerPacket->PlayerID);
					break;
				}
				case EPacketType::S_MOVE_PLAYER:
				{
					FMovePacket* MovePlayerPacket = reinterpret_cast<FMovePacket*>(RemainingData.GetData());
					UpdatePlayer(MovePlayerPacket->PlayerInfo);
					break;
				}
				case EPacketType::S_ATTACK_PLAYER :
				{
					FAttackPacket* AttackPlayerPacket = reinterpret_cast<FAttackPacket*>(RemainingData.GetData());
					UpdatePlayer(AttackPlayerPacket->PlayerInfo);
					break;
				}
				case EPacketType::S_SPAWN_MONSTER:
				{
					FSpawnMonsterPacket* SpawnMonsterPacket = reinterpret_cast<FSpawnMonsterPacket*>(RemainingData.GetData());
					SpawnMonster(SpawnMonsterPacket->MonsterInfo);
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

void UGPGameInstance::AddPlayer(FCharacterInfo& PlayerInfo, bool isMyPlayer)
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

		MyPlayer->SetCharacterInfoFromServer(PlayerInfo);
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

		Player->SetCharacterInfoFromServer(PlayerInfo);
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

void UGPGameInstance::UpdatePlayer(FCharacterInfo& PlayerInfo)
{
	auto Player = Players.Find(PlayerInfo.ID);
	if (Player)
	{
		(*Player)->SetCharacterInfoFromServer(PlayerInfo);
		UE_LOG(LogTemp, Warning, TEXT("Update other player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z, PlayerInfo.Yaw);
	}
}

void UGPGameInstance::SpawnMonster(FCharacterInfo& MonsterInfo)
{
	auto* World = GetWorld();
	if (World == nullptr)
		return;

	FVector SpawnLocation(MonsterInfo.X, MonsterInfo.Y, MonsterInfo.Z);
	FRotator SpawnRotation(0, MonsterInfo.Yaw, 0);

	AGPCharacterBase* Monster = nullptr;
	while (Monster == nullptr)
	{
		Monster = World->SpawnActor<AGPCharacterBase>(MonsterClass, SpawnLocation, SpawnRotation);
		UE_LOG(LogTemp, Warning, TEXT("Spawn Monster [%d] (%f,%f,%f)(%f)"),
			MonsterInfo.ID, MonsterInfo.X, MonsterInfo.Y, MonsterInfo.Z, MonsterInfo.Yaw);

		Monster->SetActorLocation(FVector(MonsterInfo.X, MonsterInfo.Y, MonsterInfo.Z));
		Monster->SetCharacterControl(ECharacterType::M_Mouse);
		Monster->CharacterInfo = MonsterInfo;
		Monster->Stat->SetMaxHp(MonsterInfo.MaxHp);
		Monster->Stat->SetCurrentHp(MonsterInfo.MaxHp);
		Monsters.Add(MonsterInfo.ID, Monster);
	}

}
