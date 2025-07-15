// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/GPNetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Network/GPObjectManager.h"
#include "Network/GPGameInstance.h"
#include "Character/GPCharacterPlayer.h"

bool UGPNetworkManager::ConnectToServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("ClientSocket"));
	Socket->SetNonBlocking(false); // 임시로 blocking

	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Connecting To Server..."));

	bool bConnected = Socket->Connect(*InternetAddr);
	if (bConnected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Connection Success"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Connection Failed"));
	}

	Socket->SetNonBlocking(true);
	return bConnected;
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
	UGPObjectManager* ObjectMgr = GetWorld()->GetSubsystem<UGPObjectManager>();
	if (ObjectMgr && ObjectMgr->IsChangingZone())
	{
		UE_LOG(LogTemp, Log, TEXT("Changing Zone..."));
		return;
	}

	int32 BytesSent = 0;
	Socket->Send(Buf, Size, BytesSent);
}

void UGPNetworkManager::HandleUserAuthFailure(DBResultCode ResultCode)
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

	OnUserAuthFailed.Broadcast(ErrorMessage);
}

void UGPNetworkManager::HandleBuyItemResult(bool bSuccess, uint32 CurrentGold, DBResultCode ResultCode)
{
	FString ResultMessage;

	switch (ResultCode)
	{
	case DBResultCode::SUCCESS:
		ResultMessage = TEXT("구매완료!");
		break;
	case DBResultCode::NOT_ENOUGH_GOLD:
		ResultMessage = TEXT("골드가 부족합니다");
		break;
	case DBResultCode::ITEM_NOT_FOUND:
		ResultMessage = TEXT("없는 아이템입니다");
		break;
	default:
		ResultMessage = TEXT("알 수 없는 오류가 발생했습니다");
		break;
	}

	UE_LOG(LogTemp, Error, TEXT("%s"), *ResultMessage);

	OnBuyItemResult.Broadcast(bSuccess, CurrentGold, ResultMessage);
}

void UGPNetworkManager::HandleSellItemResult(bool bSuccess, uint32 CurrentGold, DBResultCode ResultCode)
{
	FString ResultMessage;

	switch (ResultCode)
	{
	case DBResultCode::SUCCESS:
		ResultMessage = TEXT("판매완료!");
		break;
	case DBResultCode::ITEM_NOT_FOUND:
		ResultMessage = TEXT("없는 아이템입니다");
		break;
	default:
		ResultMessage = TEXT("알 수 없는 오류가 발생했습니다");
		break;
	}

	UE_LOG(LogTemp, Error, TEXT("%s"), *ResultMessage);

	OnSellItemResult.Broadcast(bSuccess, CurrentGold, ResultMessage);
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
	MovePacket Packet(info.ID, info.Pos, info.State, 0);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyAttackPacket(float PlayerYaw, FVector PlayerPos)
{
	AttackPacket Packet(PlayerYaw, PlayerPos);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyStartAiming(float PlayerYaw, FVector PlayerPos)
{
	StartAimingPacket Packet(PlayerYaw, PlayerPos);
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

void UGPNetworkManager::SendMyUseSkill(ESkillGroup SkillGID, float PlayerYaw, FVector PlayerPos)
{
	UseSkillPacket Packet(SkillGID, PlayerYaw, PlayerPos);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyZoneChangePacket(ZoneType zone)
{
	RequestZoneChangePacket Packet(zone);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyRespawnPacket(ZoneType zone)
{
	RespawnRequestPacket Packet(zone);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyShopBuyItem(uint8 ItemTypeID, int32 Quantity)
{
	BuyItemPacket Packet(ItemTypeID, Quantity);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyShopSellItem(int32 ItemID)
{
	SellItemPacket Packet(ItemID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyRequestQuest(QuestType quest)
{
	RequestQuestPacket Packet(quest);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyCompleteQuest(QuestType quest)
{
	CompleteQuestPacket Packet(quest);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyChatMessage(const FString& Message)
{
	FTCHARToUTF8 MsgUtf8(*Message);
	ChatSendPacket Packet(MsgUtf8.Get());
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyRemoveStatePacket(uint32 State)
{
	RemoveStatePacket Packet(static_cast<ECharacterStateType>(State));
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyFriendRequest(const FString& TargetNickName)
{
	FTCHARToUTF8 NameUtf8(*TargetNickName);
	FriendAddRequestPacket Packet(NameUtf8.Get());
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyFriendAccept(int32 RequesterUserID)
{
	FriendAcceptRequestPacket Packet(RequesterUserID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyFriendReject(int32 RequesterUserID)
{
	FriendRejectRequestPacket Packet(RequesterUserID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyFriendRemove(int32 TargetUserID)
{
	FriendRemoveRequestPacket Packet(TargetUserID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

//void UGPNetworkManager::SendFriendListRequest(int32 PlayerID)
//{
//	//Todo: 로드 실패시 요청해야함.
//}

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
	UGPObjectManager* ObjectMgr = GetWorld()->GetSubsystem<UGPObjectManager>();
	if (!ObjectMgr)
		return;

	ReceiveData();
	TArray<uint8> PacketData;

	while (RecvQueue.Dequeue(PacketData))
	{
		RemainingData.Append(PacketData);

		while (RemainingData.Num() > sizeof(FPacketHeader))
		{
			FPacketHeader* PacketHeader = reinterpret_cast<FPacketHeader*>(RemainingData.GetData());
			if (RemainingData.Num() < PacketHeader->PacketSize) break;
			if (ObjectMgr->IsChangingZone())
			{
				UE_LOG(LogTemp, Log, TEXT("Changing Zone..."));
				break;
			}

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
				HandleUserAuthFailure(Pkt->ResultCode);
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
				HandleUserAuthFailure(Pkt->ResultCode);
				break;
			}
			case EPacketType::S_ENTER_GAME:
			{
				EnterGamePacket* Pkt = reinterpret_cast<EnterGamePacket*>(RemainingData.GetData());
				OnEnterGame.Broadcast();
				FInfoData Data =  Pkt->PlayerInfo;
				ObjectMgr->ChangeZone(ZoneType::TUK, Data.GetZone(), Data.Pos);
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
			case EPacketType::S_PLAYER_MOVE:
				break;
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
			case EPacketType::S_PLAYER_DEAD:
			{
				PlayerDeadPacket* Pkt = reinterpret_cast<PlayerDeadPacket*>(RemainingData.GetData());
				ObjectMgr->HandlePlayerDeath(Pkt->PlayerID);
				break;
			}
#pragma endregion
#pragma region Skill
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
			case EPacketType::S_EARTH_QUAKE:
			{
				Tino::EarthQuakePacket* Pkt = reinterpret_cast<Tino::EarthQuakePacket*>(RemainingData.GetData());
				ObjectMgr->PlayEarthQuakeEffect(Pkt->RockPos, Pkt->bDebug);
				break;
			}
			case EPacketType::S_FLAME_BREATH:
			{
				Tino::FlameBreathPacket* Pkt = reinterpret_cast<Tino::FlameBreathPacket*>(RemainingData.GetData());
				ObjectMgr->PlayFlameBreathEffect(Pkt->Origin, Pkt->Direction, Pkt->Range, Pkt->AngleDeg, Pkt->bDebug);
				break;
			}
			case EPacketType::S_MONSTER_DEAD:
			{
				MonsterDeadPacket* Pkt = reinterpret_cast<MonsterDeadPacket*>(RemainingData.GetData());
				ObjectMgr->HandleMonsterDeath(Pkt->MonsterID);
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
				ObjectMgr->ItemPickUp(Pkt->ItemID);
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
				ObjectMgr->UpdatePlayer(Pkt->PlayerInfo);
				break;
			}
			case EPacketType::S_UNEQUIP_ITEM:
			{
				ItemPkt::UnequipItemPacket* Pkt = reinterpret_cast<ItemPkt::UnequipItemPacket*>(RemainingData.GetData());
				ObjectMgr->UnequipItem(Pkt->PlayerID, Pkt->ItemType);
				ObjectMgr->UpdatePlayer(Pkt->PlayerInfo);
				break;
			}
#pragma endregion
#pragma region Map
			case EPacketType::S_CHANGE_ZONE:
			{
				ChangeZonePacket* Pkt = reinterpret_cast<ChangeZonePacket*>(RemainingData.GetData());
				ZoneType OldZone = MyPlayer->CharacterInfo.CurrentZone;
				ObjectMgr->ChangeZone(OldZone, Pkt->TargetZone, Pkt->RandomPos);
				break;
			}
			case EPacketType::S_RESPAWN:
			{
				RespawnPacket* Pkt = reinterpret_cast<RespawnPacket*>(RemainingData.GetData());
				ObjectMgr->RespawnMyPlayer(Pkt->PlayerInfo);
				break;
			}
#pragma endregion
#pragma region Shop
			case EPacketType::S_SHOP_ITEM_LIST:
			{
				ShopItemListPacket* Pkt = reinterpret_cast<ShopItemListPacket*>(RemainingData.GetData());
				break;
			}
			case EPacketType::S_SHOP_BUY_RESULT:
			{
				BuyItemResultPacket* Pkt = reinterpret_cast<BuyItemResultPacket*>(RemainingData.GetData());
				HandleBuyItemResult(Pkt->bSuccess, Pkt->PlayerGold, Pkt->ResultCode);
				break;
			}
			case EPacketType::S_SHOP_SELL_RESULT:
			{
				SellItemResultPacket* Pkt = reinterpret_cast<SellItemResultPacket*>(RemainingData.GetData());
				HandleSellItemResult(Pkt->bSuccess, Pkt->PlayerGold, Pkt->ResultCode);
				break;
			}
#pragma endregion
#pragma region Quest
			case EPacketType::S_QUEST_START:
			{
				QuestStartPacket* Pkt = reinterpret_cast<QuestStartPacket*>(RemainingData.GetData());
				UE_LOG(LogTemp, Warning, TEXT("=== [Packet] S_QUEST_START received, Quest = %d ==="), static_cast<uint8>(Pkt->Quest));
				ObjectMgr->OnQuestStart(Pkt->Quest);
				break;
			}
			case EPacketType::S_QUEST_REWARD:
			{
				QuestRewardPacket* Pkt = reinterpret_cast<QuestRewardPacket*>(RemainingData.GetData());
				ObjectMgr->OnQuestReward(Pkt->Quest, Pkt->bSuccess, Pkt->ExpReward, Pkt->GoldReward);
				break;
			}
#pragma endregion
#pragma region Chat
			case EPacketType::S_CHAT_BROADCAST:
			{
				ChatBroadcastPacket* Pkt = reinterpret_cast<ChatBroadcastPacket*>(RemainingData.GetData());
				FString SenderName = UTF8_TO_TCHAR(Pkt->SenderNickName);
				FString ChatText = UTF8_TO_TCHAR(Pkt->Message);

				UE_LOG(LogTemp, Log, TEXT("%s: %s"), *SenderName, *ChatText);

				OnReceiveChat.Broadcast(SenderName, ChatText);
				break;
			}
#pragma endregion
#pragma region Friend
			case EPacketType::S_FRIEND_OPERATION_RESULT:
			{
				FriendOperationResultPacket* Pkt = reinterpret_cast<FriendOperationResultPacket*>(RemainingData.GetData());
				DBResultCode Code = Pkt->ResultCode;
				EFriendOpType OpType =  Pkt->OperationType;

				break;
			}
			case EPacketType::S_FRIEND_LIST:
			{
				FriendListPacket* Pkt = reinterpret_cast<FriendListPacket*>(RemainingData.GetData());
				uint8 Count = Pkt->FriendCount;

				UE_LOG(LogTemp, Log, TEXT("== Friend List =="));
				for (int i = 0; i < Count; ++i)
				{
					FFriendInfo& Info = Pkt->Friends[i];
					FString Name = Info.GetName();
					FString Status = Info.bAccepted ? TEXT("친구") : TEXT("요청중");
					UE_LOG(LogTemp, Log, TEXT("- %s (Lv.%d) [%s]"), *Name, Info.Level, *Status);
				}
				break;
			}
			case EPacketType::S_ADD_FRIEND:
			{
				AddFriendPacket* Pkt = reinterpret_cast<AddFriendPacket*>(RemainingData.GetData());
				FString Name = Pkt->NewFriend.GetName();
				UE_LOG(LogTemp, Log, TEXT("친구 추가: %s"), *Name);
				break;
			}
			case EPacketType::S_REMOVE_FRIEND:
			{
				RemoveFriendPacket* Pkt = reinterpret_cast<RemoveFriendPacket*>(RemainingData.GetData());
				UE_LOG(LogTemp, Log, TEXT("친구 제거됨: UserID=%d"), Pkt->FriendUserID);
				break;
			}
#pragma endregion
#pragma region Test
			case EPacketType::S_DEBUG_TRIANGLE:
			{
				DebugTrianglePacket* Packet = reinterpret_cast<DebugTrianglePacket*>(RemainingData.GetData());
				const FColor DefaultColor = FColor::White;

				DrawDebugLine(GetWorld(), Packet->A, Packet->B, DefaultColor, false, Packet->Duration, 0, 2.0f);
				DrawDebugLine(GetWorld(), Packet->B, Packet->C, DefaultColor, false, Packet->Duration, 0, 2.0f);
				DrawDebugLine(GetWorld(), Packet->C, Packet->A, DefaultColor, false, Packet->Duration, 0, 2.0f);
				break;
			}
			case EPacketType::S_DEBUG_LINE:
			{
				DebugLinePacket* Packet = reinterpret_cast<DebugLinePacket*>(RemainingData.GetData());
				const FColor DefaultColor = FColor::Green;

				DrawDebugLine(GetWorld(), Packet->A, Packet->B, DefaultColor, false, Packet->Duration, 0, 5.0f);
				break;
			}
#pragma endregion

			default:
				UE_LOG(LogTemp, Warning, TEXT("Unknown Packet Type received: type [%d] - size %d"),
					PacketHeader->PacketType, PacketHeader->PacketSize);
				break;
			}

			RemainingData.RemoveAt(0, PacketHeader->PacketSize, false);
		}
	}
}

