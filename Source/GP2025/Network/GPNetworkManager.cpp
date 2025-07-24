// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/GPNetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "Network/GPObjectManager.h"
#include "Network/GPGameInstance.h"
#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterMyPlayer.h"

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

void UGPNetworkManager::SetMyPlayer(AGPCharacterMyplayer* InMyPlayer)
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

void UGPNetworkManager::HandleUserAuthFailure(ResultCode ResCode)
{
	FString ErrorMessage;

	switch (ResCode)
	{
	case ResultCode::SUCCESS:
		return;
	case ResultCode::INVALID_USER:
		ErrorMessage = TEXT("계정을 찾을 수 없습니다");
		break;
	case ResultCode::INVALID_PASSWORD:
		ErrorMessage = TEXT("비밀번호가 일치하지 않습니다");
		break;
	case ResultCode::DUPLICATE_ID:
		ErrorMessage = TEXT("이미 존재하는 아이디입니다");
		break;
	case ResultCode::ALREADY_LOGGED_IN:
		ErrorMessage = TEXT("이미 로그인된 계정입니다");
		break;
	default:
		ErrorMessage = TEXT("알 수 없는 오류가 발생했습니다");
		break;
	}

	UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);

	OnUserAuthFailed.Broadcast(ErrorMessage);
}

void UGPNetworkManager::HandleBuyItemResult(bool bSuccess, uint32 CurrentGold, ResultCode ResCode)
{
	FString ResultMessage;

	switch (ResCode)
	{
	case ResultCode::SUCCESS:
		ResultMessage = TEXT("구매완료!");
		break;
	case ResultCode::NOT_ENOUGH_GOLD:
		ResultMessage = TEXT("골드가 부족합니다");

		break;
	case ResultCode::ITEM_NOT_FOUND:
		ResultMessage = TEXT("없는 아이템입니다");
		break;
	default:
		ResultMessage = TEXT("알 수 없는 오류가 발생했습니다");
		break;
	}

	UE_LOG(LogTemp, Error, TEXT("%s"), *ResultMessage);
	OnBuyItemResult.Broadcast(bSuccess, CurrentGold, ResultMessage);
}

void UGPNetworkManager::HandleSellItemResult(bool bSuccess, uint32 CurrentGold, ResultCode ResCode)
{
	FString ResultMessage;

	switch (ResCode)
	{
	case ResultCode::SUCCESS:
		ResultMessage = TEXT("판매완료!");
		break;
	case ResultCode::ITEM_NOT_FOUND:
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

void UGPNetworkManager::SendMyEnterGamePacket(EWorldChannel WChannel)
{
	RequestEnterGamePacket Packet(WChannel);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyEnterGamePacket(EWorldChannel WChannel, Type::EPlayer PlayerType)
{
	RequestEnterGamePacket Packet(WChannel, PlayerType);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyChangeChannelPacket(EWorldChannel WChannel)
{
	ChangeChannelRequestPacket Packet(WChannel);
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

void UGPNetworkManager::SendMyUseSkillStart(ESkillGroup SkillGID, float PlayerYaw, FVector PlayerPos)
{
	UseSkillStartPacket Packet(SkillGID, PlayerYaw, PlayerPos);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyUseSkillEnd(ESkillGroup SkillGID)
{
	UseSkillEndPacket Packet(SkillGID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyZoneChangePacket(ZoneType NewZone)
{
	MyPlayer->PlayFadeOut();
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this, NewZone]()
		{
			RequestZoneChangePacket Packet(NewZone);
			SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
		},
		0.5f, false
	);
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

void UGPNetworkManager::SendMyCompleteQuest()
{
	if (!MyPlayer)
		return;

	QuestType Quest = MyPlayer->CharacterInfo.GetCurrentQuest().QuestType;
	CompleteQuestPacket Packet(Quest);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyRejectQuest(QuestType quest)
{
	RejectQuestPacket Packet(quest);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyChatMessage(const FString& Message, EChatChannel Channel)
{
	FTCHARToUTF8 MsgUtf8(*Message);
	ChatSendPacket Packet(MsgUtf8.Get(), Channel);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyWhisperMessage(const FString& TargetName, const FString& Message)
{
	FTCHARToUTF8 NameUtf8(*TargetName);
	FTCHARToUTF8 MsgUtf8(*Message);
	ChatWhisperPacket Packet(NameUtf8.Get(), MsgUtf8.Get());
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

void UGPNetworkManager::SendMyFriendAccept(uint32 TargetDBID)
{
	FriendAcceptRequestPacket Packet(TargetDBID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyFriendReject(uint32 TargetDBID)
{
	FriendRejectRequestPacket Packet(TargetDBID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::SendMyFriendRemove(uint32 TargetDBID)
{
	FriendRemoveRequestPacket Packet(TargetDBID);
	SendPacket(reinterpret_cast<uint8*>(&Packet), sizeof(Packet));
}

void UGPNetworkManager::LoadWorldStatesFromServer(const FWorldState* ServerStates)
{
	CachedWorldStates.Empty();

	for (int i = 0; i < WORLD_MAX_COUNT; ++i)
	{
		uint8 ch = static_cast<uint8>(ServerStates[i].Channel);
		uint8 state = static_cast<uint8>(ServerStates[i].State);
		CachedWorldStates.Add(ch, state);
	}
}

void UGPNetworkManager::UpdateWorldStateFromServer(const FWorldState ServerStates)
{
		uint8 ch = static_cast<uint8>(ServerStates.Channel);
		uint8 state = static_cast<uint8>(ServerStates.State);
		CachedWorldStates[ch] = state;
}

EWorldState UGPNetworkManager::GetWorldState(EWorldChannel Channel) const
{
	if (const uint8* Found = CachedWorldStates.Find(static_cast<uint8>(Channel)))
		return static_cast<EWorldState>(*Found);
	return EWorldState::Normal;
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
				LoadWorldStatesFromServer(Pkt->WorldState);
				OnEnterLobby.Broadcast();
				break;
			}
			case EPacketType::S_LOGIN_FAIL:
			{
				LoginFailPacket* Pkt = reinterpret_cast<LoginFailPacket*>(RemainingData.GetData());
				HandleUserAuthFailure(Pkt->ResCode);
				break;
			}
			case EPacketType::S_SIGNUP_SUCCESS:
			{
				SignUpSuccessPacket* Pkt = reinterpret_cast<SignUpSuccessPacket*>(RemainingData.GetData());
				LoadWorldStatesFromServer(Pkt->WorldState);
				OnEnterLobby.Broadcast();
				break;
			}
			case EPacketType::S_SIGNUP_FAIL:
			{
				SignUpFailPacket* Pkt = reinterpret_cast<SignUpFailPacket*>(RemainingData.GetData());
				HandleUserAuthFailure(Pkt->ResCode);
				break;
			}
			case EPacketType::S_ENTER_GAME:
			{
				EnterGamePacket* Pkt = reinterpret_cast<EnterGamePacket*>(RemainingData.GetData());
				OnEnterGame.Broadcast();
				MyChannel = Pkt->WChannel;
				FString Msg = TEXT("Enter Channel [") + FString::FromInt(static_cast<uint8>(MyChannel)) + TEXT("]");
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg);

				FInfoData Data = Pkt->PlayerInfo;
				ObjectMgr->ChangeZone(ZoneType::TUK, Data.GetZone(), Data.Pos);
				ObjectMgr->AddMyPlayer(Pkt->PlayerInfo);
				break;
			}
			case EPacketType::S_CHANGE_CHANNEL:
			{
				ChangeChannelPacket* Pkt = reinterpret_cast<ChangeChannelPacket*>(RemainingData.GetData());
				if(MyChannel != Pkt->WChannel)
				{
					MyChannel = Pkt->WChannel;
					ObjectMgr->ChangeChannel(Pkt->PlayerPos);
				}
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
			case EPacketType::S_PLAYER_ATTACK:
			{
				PlayerAttackPacket* Pkt = reinterpret_cast<PlayerAttackPacket*>(RemainingData.GetData());
				ObjectMgr->PlayerAttack(Pkt->PlayerID, Pkt->PlayerPos, Pkt->PlayerYaw);
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
			case EPacketType::S_PLAYER_USE_SKILL_START:
			{
				PlayerUseSkillStartPacket* Pkt = reinterpret_cast<PlayerUseSkillStartPacket*>(RemainingData.GetData());
				ObjectMgr->PlayerUseSkillStart(Pkt->PlayerID, Pkt->SkillGID, Pkt->PlayerYaw, Pkt->PlayerPos);
				break;
			}
			case EPacketType::S_PLAYER_USE_SKILL_END:
			{
				PlayerUseSkillEndPacket* Pkt = reinterpret_cast<PlayerUseSkillEndPacket*>(RemainingData.GetData());
				ObjectMgr->PlayerUseSkillEnd(Pkt->PlayerID);
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
				HandleBuyItemResult(Pkt->bSuccess, Pkt->PlayerGold, Pkt->ResCode);
				ObjectMgr->SoundWhenBuy(Pkt->ResCode);
				break;
			}
			case EPacketType::S_SHOP_SELL_RESULT:
			{
				SellItemResultPacket* Pkt = reinterpret_cast<SellItemResultPacket*>(RemainingData.GetData());
				HandleSellItemResult(Pkt->bSuccess, Pkt->PlayerGold, Pkt->ResCode);
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
				EChatChannel Channel = Pkt->Channel;
				UE_LOG(LogTemp, Log, TEXT("%s: %s"), *SenderName, *ChatText);

				OnReceiveChat.Broadcast(static_cast<uint8>(Channel), SenderName, ChatText);
				break;
			}
#pragma endregion
#pragma region Friend
			case EPacketType::S_FRIEND_OPERATION_RESULT:
			{
				FriendOperationResultPacket* Pkt = reinterpret_cast<FriendOperationResultPacket*>(RemainingData.GetData());
				ResultCode Code = Pkt->ResCode;
				EFriendOpType OpType = Pkt->OperationType;
				{
					switch (Code)
					{
					case ResultCode::SUCCESS:
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("[Friend] Success!")));
						break;
					case ResultCode::FRIEND_ALREADY_REQUESTED:
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Friend] Already requested!")));
						break;
					case ResultCode::FRIEND_ALREADY_ADDED:
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Friend] Already added!")));
						break;
					case ResultCode::FRIEND_SELF_REQUEST:
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Friend] Cannot add yourself!")));
						break;
					case ResultCode::FRIEND_USER_NOT_FOUND:
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Friend] Target user not found!")));
						break;
					case ResultCode::DB_ERROR:
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Friend] Database error!")));
						break;
					default:
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[Friend] Unknown result code!")));
						break;
					}

					ObjectMgr->ShowFriendMessage(static_cast<int32>(Code));
				}
				break;
			}
			case EPacketType::S_ADD_FRIEND:
			{
				AddFriendPacket* Pkt = reinterpret_cast<AddFriendPacket*>(RemainingData.GetData());
				const FFriendInfo& FriendInfo = Pkt->NewFriend;
				ObjectMgr->AddFriend(FriendInfo);
				break;
			}
			case EPacketType::S_REMOVE_FRIEND:
			{
				RemoveFriendPacket* Pkt = reinterpret_cast<RemoveFriendPacket*>(RemainingData.GetData());
				ObjectMgr->RemoveFriend(Pkt->FriendUserID);
				break;
			}
			case EPacketType::S_REQUEST_FRIEND:
			{
				FriendRequestPacket* Pkt = reinterpret_cast<FriendRequestPacket*>(RemainingData.GetData());
				const FFriendInfo& FriendInfo = Pkt->RequesterInfo;
				ObjectMgr->AddRequestFriend(FriendInfo);
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
			case EPacketType::S_WORLD_STATE:
			{
				ChangedWorldStatePacket* Pkt = reinterpret_cast<ChangedWorldStatePacket*>(RemainingData.GetData());
				UpdateWorldStateFromServer(Pkt->WorldState);
				break;
			}
			default:
				UE_LOG(LogTemp, Warning, TEXT("Unknown Packet Type received: type [%d] - size %d"),
					PacketHeader->PacketType, PacketHeader->PacketSize);
				break;
			}

			RemainingData.RemoveAt(0, PacketHeader->PacketSize, false);
		}
	}
}

