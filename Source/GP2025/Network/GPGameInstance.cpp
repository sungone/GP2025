// Fill out your copyright notice in the Description page of Project Settings.

#include "Network/GPGameInstance.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterMonster.h"
#include "UI/GPFloatingDamageText.h"

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
	IDPacket Packet(EPacketType::C_LOGOUT, MyPlayer->CharacterInfo.ID);
	int32 BytesSent = 0;
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(IDPacket), BytesSent);
}

void UGPGameInstance::SendPlayerMovePacket()
{
	InfoPacket Packet(EPacketType::C_MOVE, MyPlayer->CharacterInfo);
	int32 BytesSent = 0;
	UE_LOG(LogTemp, Warning, TEXT("SendPlayerMovePacket : Send [%d]"), Packet.Data.ID);
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(InfoPacket), BytesSent);
}

void UGPGameInstance::SendPlayerAttackPacket(int32 TargetID)
{
	AttackPacket Packet(TargetID);
	int32 BytesSent = 0;
	UE_LOG(LogTemp, Warning, TEXT("SendPlayerAttackPacket : Send [%d]"), MyPlayer->CharacterInfo.ID);
	Socket->Send(reinterpret_cast<uint8*>(&Packet), sizeof(AttackPacket), BytesSent);
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
				case EPacketType::S_LOGIN_SUCCESS:
				{
					InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
					AddPlayer(Pkt->Data, true);
					break;
				}
				case EPacketType::S_ADD_PLAYER:
				{
					InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
					AddPlayer(Pkt->Data, false);
					break;
				}
				case EPacketType::S_REMOVE_PLAYER:
				{
					IDPacket* Pkt = reinterpret_cast<IDPacket*>(RemainingData.GetData());
					RemovePlayer(Pkt->Data);
					break;
				}
				case EPacketType::S_PLAYER_STATUS_UPDATE:
				{
					InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
					UpdatePlayer(Pkt->Data);
					break;
				}
				case EPacketType::S_ADD_MONSTER:
				{
					InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
					AddMonster(Pkt->Data);
					break;
				}
				case EPacketType::S_REMOVE_MONSTER:
				{
					IDPacket* Pkt = reinterpret_cast<IDPacket*>(RemainingData.GetData());
					RemoveMonster(Pkt->Data);
					break;
				}
				case EPacketType::S_MONSTER_STATUS_UPDATE:
				{
					InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
					UpdateMonster(Pkt->Data);
					break;
				}case EPacketType::S_DAMAGED_MONSTER:
				{
					DamagePacket* Pkt = reinterpret_cast<DamagePacket*>(RemainingData.GetData());
					DamagedMonster(Pkt->Target,Pkt->Damage);
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

void UGPGameInstance::AddPlayer(FInfoData& PlayerInfo, bool isMyPlayer)
{
	auto* World = GetWorld();
	if (World == nullptr)
		return;

	FVector SpawnLocation(PlayerInfo.Pos);
	FRotator SpawnRotation(0, PlayerInfo.Yaw, 0);

	if (isMyPlayer)
	{
		if (MyPlayer == nullptr)
			return;

		UE_LOG(LogTemp, Warning, TEXT("Add my player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.Pos.X, PlayerInfo.Pos.Y, PlayerInfo.Pos.Z, PlayerInfo.Yaw);

		MyPlayer->SetCharacterInfo(PlayerInfo);
		MyPlayer->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
		Players.Add(PlayerInfo.ID, MyPlayer);
	}
	else
	{
		AGPCharacterViewerPlayer* Player = nullptr;
		while (Player == nullptr)
		{
			Player = World->SpawnActor<AGPCharacterViewerPlayer>(OtherPlayerClass, SpawnLocation, SpawnRotation);
		}

		UE_LOG(LogTemp, Warning, TEXT("Add other player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.Pos.X, PlayerInfo.Pos.Y, PlayerInfo.Pos.Z, PlayerInfo.Yaw);

		Player->SetCharacterInfo(PlayerInfo);
		Player->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
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

void UGPGameInstance::UpdatePlayer(FInfoData& PlayerInfo)
{
	auto Player = Players.Find(PlayerInfo.ID);
	if (Player)
	{
		(*Player)->SetCharacterInfo(PlayerInfo);
		UE_LOG(LogTemp, Warning, TEXT("Update other player [%d] (%f,%f,%f)(%f)"),
			PlayerInfo.ID, PlayerInfo.Pos.X, PlayerInfo.Pos.Y, PlayerInfo.Pos.Z, PlayerInfo.Yaw);
	}
}

void UGPGameInstance::AddMonster(FInfoData& MonsterInfo)
{
	auto* World = GetWorld();
	if (World == nullptr)
		return;

	FVector SpawnLocation(MonsterInfo.Pos);
	FRotator SpawnRotation(0, MonsterInfo.Yaw, 0);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AGPCharacterMonster* Monster = World->SpawnActor<AGPCharacterMonster>(MonsterClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Monster == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn monster [%d] at location (%f, %f, %f)."),
			MonsterInfo.ID, SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawned Monster [%d] at (%f, %f, %f) with rotation (%f)."),
		MonsterInfo.ID, SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z, SpawnRotation.Yaw);

	Monster->SetCharacterInfo(MonsterInfo);
	Monster->SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
	Monster->SetCharacterType(MonsterInfo.CharacterType);
	Monsters.Add(MonsterInfo.ID, Monster);
}

void UGPGameInstance::RemoveMonster(int32 MonsterID)
{
	UE_LOG(LogTemp, Warning, TEXT("Remove monster [%d]"), MonsterID);
	auto Monster = Monsters.Find(MonsterID);
	if (Monster)
	{
		(*Monster)->SetDead();
		return;
	}
}

void UGPGameInstance::UpdateMonster(FInfoData& MonsterInfo)
{
	auto Monster = Monsters.Find(MonsterInfo.ID);
	if (Monster)
	{
		if (MonsterInfo.HasState(ECharacterStateType::STATE_DIE))
		{
			return;
		}
		(*Monster)->SetCharacterInfo(MonsterInfo);
		UE_LOG(LogTemp, Warning, TEXT("Update monster [%d]"), MonsterInfo.ID);
	}
}

void UGPGameInstance::DamagedMonster(FInfoData& MonsterInfo, float Damage)
{
	auto Monster = Monsters.Find(MonsterInfo.ID);

	if (Monster)
	{
		(*Monster)->SetCharacterInfo(MonsterInfo);

		// Floating Damage UI
		{
			FVector SpawnLocation = (*Monster)->GetActorLocation() + FVector(0, 0, 100);
			FActorSpawnParameters SpawnParams;
			AGPFloatingDamageText* DamageText = GetWorld()->SpawnActor<AGPFloatingDamageText>(AGPFloatingDamageText::StaticClass(),
				SpawnLocation, FRotator::ZeroRotator, SpawnParams);

			bool isCrt = (MyPlayer->CharacterInfo.GetDamage() != Damage);

			if (DamageText)
			{
				DamageText->SetDamageText(Damage, isCrt);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Damaged monster [%d]"), MonsterInfo.ID);
	}
}
