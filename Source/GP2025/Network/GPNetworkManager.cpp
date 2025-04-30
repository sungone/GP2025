// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/GPNetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Network/GPObjectManager.h"
#include "Network/GPGameInstance.h"
#include "Character/GPCharacterPlayer.h"

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
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void UGPNetworkManager::DisconnectFromServer()
{
	if (Socket)
	{
		this->SendMyLogoutPacket();
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
}

void UGPNetworkManager::SetMyPlayer(AGPCharacterPlayer* InMyPlayer)
{
	MyPlayer = InMyPlayer;
	UGPObjectManager* ObjectMgr = GetWorld()->GetSubsystem<UGPObjectManager>();
	ObjectMgr->SetMyPlayer(InMyPlayer);
}

void UGPNetworkManager::SendPacket(uint8* Buf, int32 Size)
{
	int32 BytesSent = 0;
	Socket->Send(Buf, Size, BytesSent);
}

void UGPNetworkManager::PrintFailMessege(DBResultCode ResultCode)
{
	FString ErrorMessage;

	switch (ResultCode)
	{
	case DBResultCode::SUCCESS:
		return;
	case DBResultCode::INVALID_USER:
		ErrorMessage = TEXT("계정을 찾을 수 없습니다");
		break;
	case DBResultCode::INVALID_PASSWORD:
		ErrorMessage = TEXT("비밀번호가 일치하지 않습니다");
		break;
	case DBResultCode::DUPLICATE_ID:
		ErrorMessage = TEXT("이미 존재하는 아이디입니다");
		break;
	default:
		ErrorMessage = TEXT("알 수 없는 오류가 발생했습니다");
		break;
	}

	UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);

	OnLoginFailed.Broadcast(ErrorMessage);
}

void UGPNetworkManager::SendMyLoginPacket(const FString& AccountID, const FString& AccountPW)
{
	FTCHARToUTF8 IDUtf8(*AccountID);
	FTCHARToUTF8 PWUtf8(*AccountPW);
	LoginPacket Packet(IDUtf8.Get(), PWUtf8.Get());
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMySignUpPacket(const FString& AccountID, const FString& AccountPW, const FString& NickName)
{
	FTCHARToUTF8 IDUtf8(*AccountID);
	FTCHARToUTF8 PWUtf8(*AccountPW);
	FTCHARToUTF8 NickUtf8(*NickName);
	SignUpPacket Packet(IDUtf8.Get(), PWUtf8.Get(), NickUtf8.Get());
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}


void UGPNetworkManager::SendMyLogoutPacket()
{
	if (!MyPlayer)
		return;
	IDPacket Packet(EPacketType::C_LOGOUT, MyPlayer->CharacterInfo.ID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMySelectCharacter(uint8 PlayerType)
{
	SelectCharacterPacket Packet((Type::EPlayer)PlayerType);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyEnterGamePacket()
{
	RequestEnterGamePacket Packet;
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyEnterGamePacket(Type::EPlayer PlayerType)
{
	RequestEnterGamePacket Packet(PlayerType);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyMovePacket()
{
	auto info = MyPlayer->CharacterInfo;
	MovePacket Packet(info.ID,info.Pos,info.State,0);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyAttackPacket(float PlayerYaw)
{
	AttackPacket Packet(PlayerYaw);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyStartAiming(float PlayerYaw)
{
	StartAimingPacket Packet(PlayerYaw);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyStopAiming()
{
	StopAimingPacket Packet;
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyTakeItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_TAKE_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyDropItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_DROP_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyUseItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_USE_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyEquipItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_EQUIP_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyUnequipItem(int32 ItemID)
{
	IDPacket Packet(EPacketType::C_UNEQUIP_ITEM, ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyUseSkill(ESkillGroup SkillGID, float PlayerYaw)
{
	UseSkillPacket Packet(SkillGID, PlayerYaw);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::ReceiveData()
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

void UGPNetworkManager::ProcessPacket()
{
	ReceiveData();
	TArray<uint8> PacketData;
	UGPObjectManager* ObjectMgr = GetWorld()->GetSubsystem<UGPObjectManager>();
	while (RecvQueue.Dequeue(PacketData))
	{
		RemainingData.Append(PacketData);

		while (RemainingData.Num() > sizeof(FPacketHeader))
		{
			FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(RemainingData.GetData());

			if (RemainingData.Num() < PacketHeader->PacketSize) break;

			switch (PacketHeader->PacketType)
			{
#pragma region Player
			case EPacketType::S_LOGIN_SUCCESS:
			{
				LoginSuccessPacket* Pkt = reinterpret_cast<LoginSuccessPacket*>(RemainingData.GetData());

				OnEnterLobby.Broadcast();

				// Todo: 입장버튼 클릭시 호출되도록
				break;
			}
			case EPacketType::S_LOGIN_FAIL:
			{
				LoginFailPacket* Pkt = reinterpret_cast<LoginFailPacket*>(RemainingData.GetData());
				PrintFailMessege(Pkt->ResultCode);
				break;
			}
			case EPacketType::S_SIGNUP_SUCCESS:
			{
				SignUpSuccessPacket* Pkt = reinterpret_cast<SignUpSuccessPacket*>(RemainingData.GetData());
				OnEnterLobby.Broadcast();
				break;
			}
			case EPacketType::S_SIGNUP_FAIL:
			{
				SignUpFailPacket* Pkt = reinterpret_cast<SignUpFailPacket*>(RemainingData.GetData());
				PrintFailMessege(Pkt->ResultCode);
				break;
			}
			case EPacketType::S_ENTER_GAME:
			{
				EnterGamePacket* Pkt = reinterpret_cast<EnterGamePacket*>(RemainingData.GetData());
				OnEnterGame.Broadcast();
				ObjectMgr->AddMyPlayer(Pkt->PlayerInfo);
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
			case EPacketType::S_SKILL_UNLOCK:
			{
				SkillUnlockPacket* Pkt = reinterpret_cast<SkillUnlockPacket*>(RemainingData.GetData());
				ObjectMgr->SkillUnlock(Pkt->SkillGID);
				break;
			}
			case EPacketType::S_SKILL_UPGRADE:
			{
				UpgradeSkillPacket* Pkt = reinterpret_cast<UpgradeSkillPacket*>(RemainingData.GetData());
				ObjectMgr->SkillUpgrade(Pkt->SkillGID);
				break;
			}
			case EPacketType::S_PLAYER_USE_SKILL:
			{
				PlayerUseSkillPacket* Pkt = reinterpret_cast<PlayerUseSkillPacket*>(RemainingData.GetData());
				ObjectMgr->PlayerUseSkill(Pkt->PlayerID, Pkt->SkillGID);
				break;
			}
			case EPacketType::S_LEVEL_UP:
			{
				PlayerLevelUpPacket* Pkt = reinterpret_cast<PlayerLevelUpPacket*>(RemainingData.GetData());
				ObjectMgr->LevelUp(Pkt->PlayerInfo);
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

			RemainingData.RemoveAt(0, PacketHeader->PacketSize, false);
		}
	}
}

