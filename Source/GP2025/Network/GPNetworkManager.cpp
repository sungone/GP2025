// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/GPNetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Network/GPObjectManager.h"
#include "Network/GPGameInstance.h"
#include "Character/GPCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"

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
		WorkerRunnable = MakeShareable(new FWorkerThread(this));
		WorkerThread = FRunnableThread::Create(WorkerRunnable.Get(), TEXT("GPRecvWorkerThread"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void UGPNetworkManager::DisconnectFromServer()
{
	if (WorkerRunnable.IsValid())
	{
		WorkerRunnable->Stop();
	}
	if (WorkerThread)
	{
		WorkerThread->Kill(true);
		delete WorkerThread;
		WorkerThread = nullptr;
	}

	WorkerRunnable.Reset();

	if (Socket)
	{
		this->SendPlayerLogoutPacket();
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("Disconnected from server and thread cleaned up."));
}


void UGPNetworkManager::SetMyPlayer()
{
	if (!GetWorld()) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	AGPCharacterPlayer* Player = Cast<AGPCharacterPlayer>(PC->GetPawn());
	if (!Player) return;

	MyPlayer = Player;

	if (UGPObjectManager* ObjectMgr = GetWorld()->GetSubsystem<UGPObjectManager>())
	{
		ObjectMgr->SetMyPlayer(Player);
	}
}

void UGPNetworkManager::SendPacket(uint8* Buf, int32 Size)
{
	int32 BytesSent = 0;
	Socket->Send(Buf, Size, BytesSent);
}

void UGPNetworkManager::SendPlayerLoginPacket(const FString& AccountID, const FString& AccountPW)
{
	FTCHARToUTF8 IDUtf8(*AccountID);
	FTCHARToUTF8 PWUtf8(*AccountPW);
	LoginPacket Packet(IDUtf8.Get(), PWUtf8.Get());
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendPlayerSignUpPacket(const FString& AccountID, const FString& AccountPW, const FString& NickName)
{
	FTCHARToUTF8 IDUtf8(*AccountID);
	FTCHARToUTF8 PWUtf8(*AccountPW);
	FTCHARToUTF8 NickUtf8(*NickName);
	SignUpPacket Packet(IDUtf8.Get(), PWUtf8.Get(), NickUtf8.Get());
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}


void UGPNetworkManager::SendPlayerLogoutPacket()
{
	if (!MyPlayer)
		return;
	IDPacket Packet(EPacketType::C_LOGOUT, MyPlayer->CharacterInfo.ID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendPlayerMovePacket()
{
	InfoPacket Packet(EPacketType::C_MOVE, MyPlayer->CharacterInfo);
	UE_LOG(LogTemp, Warning, TEXT("SendPlayerMovePacket : Send [%d]"), Packet.Data.ID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendPlayerAttackPacket(float PlayerYaw)
{
	AttackPacket Packet(PlayerYaw);
	UE_LOG(LogTemp, Warning, TEXT("SendPlayerAttackPacket : Send [%d]"), MyPlayer->CharacterInfo.ID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendPlayerTakeItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_TAKE_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendPlayerDropItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_DROP_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendPlayerUseItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_USE_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendPlayerEquipItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_EQUIP_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendPlayerUnequipItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_UNEQUIP_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::ReceiveData()
{
	uint32 DataSize;
	if (!Socket) return;
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
	while (RecvQueue.Dequeue(PacketData))
	{
		RemainingData.Append(PacketData);

		while (RemainingData.Num() > sizeof(FPacketHeader))
		{
			FPacketHeader* Header = reinterpret_cast<FPacketHeader*>(RemainingData.GetData());

			if (RemainingData.Num() < Header->PacketSize || Header->PacketSize <= 0)
				break;
			TArray<uint8> PacketCopy;
			PacketCopy.Append(RemainingData.GetData(), Header->PacketSize);

			AsyncTask(ENamedThreads::GameThread, [this, PacketCopy]()
				{
					HandlePacketOnGameThread(PacketCopy);
				});

			RemainingData.RemoveAt(0, Header->PacketSize, false);
		}
	}
}

void UGPNetworkManager::HandlePacketOnGameThread(const TArray<uint8>& PacketData)
{
	if (!GetWorld()) return;
	UGPObjectManager* ObjectMgr = GetWorld()->GetSubsystem<UGPObjectManager>();
	if (!ObjectMgr) return;
	const FPacketHeader* Header = reinterpret_cast<const FPacketHeader*>(PacketData.GetData());

	switch (Header->PacketType)
	{
#pragma region Player
	case EPacketType::S_LOGIN_SUCCESS:
	{
		LoginSuccessPacket* Pkt = reinterpret_cast<LoginSuccessPacket*>(RemainingData.GetData());
		CachedLoginInfo = Pkt->PlayerInfo;
		Cast<UGPGameInstance>(GetGameInstance())->OnLoginSuccess();
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, [this]()
			{
				if (UGPObjectManager* ObjMgr = GetWorld()->GetSubsystem<UGPObjectManager>())
				{
					ObjMgr->OnLoginSuccess(CachedLoginInfo);
				}
			}, 1.0f, false);
		break;
	}
	case EPacketType::S_LOGIN_FAIL:
	{
		LoginFailPacket* Pkt = reinterpret_cast<LoginFailPacket*>(RemainingData.GetData());
		ObjectMgr->PrintFailMessege(Pkt->ResultCode);
		break;
	}
	case EPacketType::S_SIGNUP_SUCCESS:
	{
		SignUpSuccessPacket* Pkt = reinterpret_cast<SignUpSuccessPacket*>(RemainingData.GetData());
		Cast<UGPGameInstance>(GetGameInstance())->OnLoginSuccess();
		ObjectMgr->OnLoginSuccess(Pkt->PlayerInfo);
		break;
	}
	case EPacketType::S_SIGNUP_FAIL:
	{
		SignUpFailPacket* Pkt = reinterpret_cast<SignUpFailPacket*>(RemainingData.GetData());
		ObjectMgr->PrintFailMessege(Pkt->ResultCode);
		break;
	}
	case EPacketType::S_ADD_PLAYER:
	{
		InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(RemainingData.GetData());
		ObjectMgr->AddPlayer(Pkt->Data);
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
	case EPacketType::S_DAMAGED_PLAYER:
	{
		PlayerDamagePacket* Pkt = reinterpret_cast<PlayerDamagePacket*>(RemainingData.GetData());
		ObjectMgr->DamagedPlayer(Pkt->Target);
		break;
	}
#pragma endregion
#pragma region Monster
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
		MonsterDamagePacket* Pkt = reinterpret_cast<MonsterDamagePacket*>(RemainingData.GetData());
		ObjectMgr->DamagedMonster(Pkt->Target, Pkt->Damage);
		break;
	}
#pragma endregion
#pragma region Item
	case EPacketType::S_ITEM_SPAWN:
	{
		ItemPkt::SpawnPacket* Pkt = reinterpret_cast<ItemPkt::SpawnPacket*>(RemainingData.GetData());
		ObjectMgr->ItemSpawn(Pkt->ItemID, Pkt->ItemType, Pkt->Pos);
		break;
	}
	case EPacketType::S_ITEM_DESPAWN:
	{
		ItemPkt::DespawnPacket* Pkt = reinterpret_cast<ItemPkt::DespawnPacket*>(RemainingData.GetData());
		ObjectMgr->ItemDespawn(Pkt->ItemID);
		break;
	}
	case EPacketType::S_ITEM_PICKUP:
	{
		ItemPkt::PickUpPacket* Pkt = reinterpret_cast<ItemPkt::PickUpPacket*>(RemainingData.GetData());
		//Todo: 추후 pick up이랑 despawn 구분하기
		ObjectMgr->ItemDespawn(Pkt->ItemID);
		break;
	}
	case EPacketType::S_ITEM_DROP:
	{
		ItemPkt::DropPacket* Pkt = reinterpret_cast<ItemPkt::DropPacket*>(RemainingData.GetData());
		ObjectMgr->DropItem(Pkt->ItemID, Pkt->ItemType, Pkt->Pos);
		break;
	}
	case EPacketType::S_ADD_INVENTORY_ITEM:
	{
		ItemPkt::AddInventoryPacket* Pkt = reinterpret_cast<ItemPkt::AddInventoryPacket*>(RemainingData.GetData());
		ObjectMgr->AddInventoryItem(Pkt->ItemID, Pkt->ItemType);
		break;
	}
	case EPacketType::S_USE_INVENTORY_ITEM:
	{
		ItemPkt::ItemUsedPacket* Pkt = reinterpret_cast<ItemPkt::ItemUsedPacket*>(RemainingData.GetData());
		ObjectMgr->UseInventoryItem(Pkt->ItemID);
		ObjectMgr->UpdatePlayer(Pkt->PlayerInfo);
		break;
	}
	case EPacketType::S_EQUIP_ITEM:
	{
		ItemPkt::EquipItemPacket* Pkt = reinterpret_cast<ItemPkt::EquipItemPacket*>(RemainingData.GetData());
		ObjectMgr->EquipItem(Pkt->PlayerID, Pkt->ItemType);
		break;
	}
	case EPacketType::S_UNEQUIP_ITEM:
	{
		ItemPkt::UnequipItemPacket* Pkt = reinterpret_cast<ItemPkt::UnequipItemPacket*>(RemainingData.GetData());
		ObjectMgr->UnequipItem(Pkt->PlayerID, Pkt->ItemType);
		break;
	}
#pragma endregion
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown Packet Type received."));
		break;
	}
}

