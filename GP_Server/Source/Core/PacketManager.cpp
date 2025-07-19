#include "pch.h"
#include "PacketManager.h"
#include "magic_enum/magic_enum.hpp"

void PacketManager::ProcessPacket(int32 sessionId, Packet* packet)
{
	EPacketType packetType = static_cast<EPacketType>(packet->Header.PacketType);
	auto name = std::string(magic_enum::enum_name(packetType));
	if (name.empty()) name = "Unknown";

	LOG_D("{} from [{}]", name, sessionId);

	switch (packetType)
	{
	case EPacketType::C_SIGNUP:
		HandleSignUpPacket(sessionId, packet);
		break;
	case EPacketType::C_LOGIN:
		HandleLoginPacket(sessionId, packet);
		break;
	case EPacketType::C_LOGOUT:
		HandleLogoutPacket(sessionId);
		break;
	case EPacketType::C_SELECT_CHARACTER:
		HandleSelectCharacterPacket(sessionId, packet);
		break;
	case EPacketType::C_ENTER_GAME:
		HandleEnterGamePacket(sessionId, packet);
		break;

	case EPacketType::C_MOVE:
		HandleMovePacket(sessionId, packet);
		break;
	case EPacketType::C_ATTACK:
		HandleAttackPacket(sessionId, packet);
		break;
	case EPacketType::C_START_AIMING:
		HandleStartAimingPacket(sessionId, packet);
		break;
	case EPacketType::C_STOP_AIMING:
		HandleStopAimingPacket(sessionId, packet);
		break;

	case EPacketType::C_USE_SKILL:
		HandleUseSkillPacket(sessionId, packet);
		break;
	case EPacketType::C_REMOVE_STATE:
		HandleRemoveStatePacket(sessionId, packet);
		break;


	case EPacketType::C_TAKE_ITEM:
		HandlePickUpItemPacket(sessionId, packet);
		break;
	case EPacketType::C_DROP_ITEM:
		HandleDropItemPacket(sessionId, packet);
		break;
	case EPacketType::C_USE_ITEM:
		HandleUseItemPacket(sessionId, packet);
		break;
	case EPacketType::C_EQUIP_ITEM:
		HandleEquipItemPacket(sessionId, packet);
		break;
	case EPacketType::C_UNEQUIP_ITEM:
		HandleUnequipItemPacket(sessionId, packet);
		break;


	case EPacketType::C_CHANGE_ZONE:
		HandleZoneChangeRequestPacket(sessionId, packet);
		break;
	case EPacketType::C_RESPAWN:
		HandleRespawnRequestPacket(sessionId, packet);
		break;

	case EPacketType::C_SHOP_BUY_ITEM:
		HandleShopBuyItemPacket(sessionId, packet);
		break;
	case EPacketType::C_SHOP_SELL_ITEM:
		HandleShopSellItemPacket(sessionId, packet);
		break;


	case EPacketType::C_REQUEST_QUEST:
		HandleRequestQuestPacket(sessionId, packet);
		break;
	case EPacketType::C_COMPLETE_QUEST:
		HandleCompleteQuestPacket(sessionId, packet);
		break;
	case EPacketType::C_REJECT_QUEST:
		HandleRejectQuestPacket(sessionId, packet);
		break;

	case EPacketType::C_CHAT_SEND:
		HandleChatSendPacket(sessionId, packet);
		break;
	case EPacketType::C_CHAT_WHISPER:
		HandleChatWhisperPacket(sessionId, packet);
		break;


	case EPacketType::C_FRIEND_REQUEST:
		HandleFriendAddRequestPacket(sessionId, packet);
		break;
	case EPacketType::C_FRIEND_ACCEPT:
		HandleFriendAcceptRequestPacket(sessionId, packet);
		break;
	case EPacketType::C_FRIEND_REMOVE:
		HandleFriendRemoveRequestPacket(sessionId, packet);
		break;
	case EPacketType::C_FRIEND_REJECT:
		HandleFriendRejectRequestPacket(sessionId, packet);
		break;
	}
}

void PacketManager::HandleSignUpPacket(int32 sessionId, Packet* packet)
{
	auto pkt = static_cast<SignUpPacket*>(packet);
#ifdef DB_MODE
	std::wstring name = ConvertToWString(pkt->NickName);
	auto id = pkt->AccountID;
	auto pw = pkt->AccountPW;
	auto res = DBManager::GetInst().SignUpUser(sessionId, id, pw, name);

	SessionManager::GetInst().Schedule(sessionId, [sessionId, res]() {
		if (res.code != ResultCode::SUCCESS)
		{
			LOG_D("SignUp Failed [{}]", sessionId);
			SignUpFailPacket failpkt(res.code);
			SessionManager::GetInst().SendPacket(sessionId, &failpkt);
			return;
		}

		SessionManager::GetInst().HandleLogin(sessionId, res);
		LOG_D("SignUp Success [{}] userId: {}", sessionId, res.dbId);
		});
	return;
#else
	LOG_D("ID: {}, PW: {}", pkt->AccountID, pkt->AccountPW);
	_sessionMgr.HandleLogin(sessionId);
	LoginSuccessPacket loginpkt;
	_sessionMgr.SendPacket(sessionId, &loginpkt);
#endif
}

void PacketManager::HandleLoginPacket(int32 sessionId, Packet* packet)
{
	auto pkt = static_cast<LoginPacket*>(packet);
#ifdef DB_MODE
	auto accountID = pkt->AccountID;
	auto accountPW = pkt->AccountPW;
	auto res = DBManager::GetInst().CheckLogin(sessionId, accountID, accountPW);
	auto loginsess = _sessionMgr.GetOnlineSessionIdByDBId(res.dbId);
	if (loginsess != -1 && loginsess != sessionId)
	{
		LOG_W("User already logged in [{}] - Disconnecting old session: {}", res.dbId, loginsess);
		res.code = ResultCode::ALREADY_LOGGED_IN;
		LoginFailPacket failpkt(res.code);
		SessionManager::GetInst().SendPacket(sessionId, &failpkt);
		return;
	}

	SessionManager::GetInst().Schedule(sessionId, [sessionId, res]() {
		if (res.code != ResultCode::SUCCESS)
		{
			LoginFailPacket failpkt(res.code);
			SessionManager::GetInst().SendPacket(sessionId, &failpkt);
			return;
		}

		LoginSuccessPacket loginpkt;
		SessionManager::GetInst().SendPacket(sessionId, &loginpkt);
		SessionManager::GetInst().HandleLogin(sessionId, res);

		LOG_D("Login Success [{}] userId: {}", sessionId, res.dbId);
		});
	return;

#else
	LOG_D("ID: {}, PW: {}", pkt->AccountID, pkt->AccountPW);
	_sessionMgr.HandleLogin(sessionId);
	LoginSuccessPacket loginpkt;
	_sessionMgr.SendPacket(sessionId, &loginpkt);
#endif
}

void PacketManager::HandleLogoutPacket(int32 sessionId)
{
	auto pkt = IDPacket(EPacketType::S_REMOVE_PLAYER, sessionId);
	//Todo: 로그인 전에 종료되면 여기서  "Invalid" 호출되는 것 같은데 해결하자
	_gameWorld.PlayerLeaveGame(sessionId);
	_sessionMgr.Disconnect(sessionId);
}

void PacketManager::HandleSelectCharacterPacket(int32 sessionId, Packet* packet)
{
	SelectCharacterPacket* p = static_cast<SelectCharacterPacket*>(packet);
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsLogin()) return;

	auto player = session->GetPlayer();
	if (!player) return;

	player->SetCharacterType(p->PlayerType);

	if (session->IsInGame())
	{
		InfoPacket infopkt(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
		_sessionMgr.SendPacket(sessionId, &infopkt);
	}
}

void PacketManager::HandleEnterGamePacket(int32 sessionId, Packet* packet)
{
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsLogin()) return;

	auto player = session->GetPlayer();
	if (!player) return;
	RequestEnterGamePacket* p = static_cast<RequestEnterGamePacket*>(packet);
	auto newType = p->PlayerType;
	if (newType != Type::EPlayer::NONE)
		player->SetCharacterType(p->PlayerType);

	session->EnterGame();
	_gameWorld.PlayerEnterGame(player);
	uint32 dbId = session->GetDBID();
	auto [friendResultCode, friendList] = DBManager::GetInst().GetFriendList(dbId);
	if (friendResultCode != ResultCode::SUCCESS)
	{
		LOG_W("Failed to load friend list for user: {}", dbId);
	}
	session->SetAndSendFriendsInfo(friendList);
}

void PacketManager::HandleMovePacket(int32 sessionId, Packet* packet)
{
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsInGame()) return;

	MovePacket* p = static_cast<MovePacket*>(packet);
	_gameWorld.PlayerMove(p->PlayerID, p->PlayerPos, p->State, p->MoveTime);
}

void PacketManager::HandleStartAimingPacket(int32 sessionId, Packet* packet)
{
	StartAimingPacket* p = static_cast<StartAimingPacket*>(packet);
	_gameWorld.PlayerSetLocation(sessionId, p->PlayerYaw, p->PlayerPos);
	_gameWorld.PlayerAddState(sessionId, ECharacterStateType::STATE_AIMING);
}

void PacketManager::HandleStopAimingPacket(int32 sessionId, Packet* packet)
{
	StopAimingPacket* p = static_cast<StopAimingPacket*>(packet);
	_gameWorld.PlayerRemoveState(sessionId, ECharacterStateType::STATE_AIMING);
}

void PacketManager::HandleAttackPacket(int32 sessionId, Packet* packet)
{
	AttackPacket* p = static_cast<AttackPacket*>(packet);
	_gameWorld.PlayerSetLocation(sessionId, p->PlayerYaw, p->PlayerPos);
	_gameWorld.PlayerAddState(sessionId, ECharacterStateType::STATE_AUTOATTACK);
	_gameWorld.PlayerAttack(sessionId);
}

void PacketManager::HandleUseSkillPacket(int32 sessionId, Packet* packet)
{
	UseSkillPacket* p = static_cast<UseSkillPacket*>(packet);
	_gameWorld.PlayerSetLocation(sessionId, p->PlayerYaw, p->PlayerPos);
	_gameWorld.PlayerUseSkill(sessionId, p->SkillGID);
}

void PacketManager::HandleRemoveStatePacket(int32 sessionId, Packet* packet)
{
	RemoveStatePacket* p = static_cast<RemoveStatePacket*>(packet);
	_gameWorld.PlayerRemoveState(sessionId, p->State);
}

void PacketManager::HandlePickUpItemPacket(int32 sessionId, Packet* packet)
{
	IDPacket* p = static_cast<IDPacket*>(packet);
	auto itemid = p->Data;
	_gameWorld.PickUpWorldItem(sessionId, itemid);
}

void PacketManager::HandleDropItemPacket(int32 sessionId, Packet* packet)
{
	IDPacket* p = static_cast<IDPacket*>(packet);
	auto itemid = p->Data;
	//_gameWorld.DropInventoryItem(sessionId, itemid);
}

void PacketManager::HandleUseItemPacket(int32 sessionId, Packet* packet)
{
	IDPacket* p = static_cast<IDPacket*>(packet);
	auto itemid = p->Data;
	_gameWorld.UseInventoryItem(sessionId, itemid);
}

void PacketManager::HandleEquipItemPacket(int32 sessionId, Packet* packet)
{
	IDPacket* p = static_cast<IDPacket*>(packet);
	auto itemid = p->Data;
	_gameWorld.EquipInventoryItem(sessionId, itemid);
}

void PacketManager::HandleUnequipItemPacket(int32 sessionId, Packet* packet)
{
	IDPacket* p = static_cast<IDPacket*>(packet);
	auto itemid = p->Data;
	_gameWorld.UnequipInventoryItem(sessionId, itemid);
}

void PacketManager::HandleZoneChangeRequestPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<RequestZoneChangePacket*>(packet);
	ZoneType targetZone = p->TargetZone;

	_gameWorld.TransferToZone(sessionId, targetZone);
}

void PacketManager::HandleRespawnRequestPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<RespawnRequestPacket*>(packet);
	ZoneType targetZone = p->TargetZone;
	_gameWorld.RespawnPlayer(sessionId, targetZone);
}

void PacketManager::HandleShopBuyItemPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<BuyItemPacket*>(packet);
	_gameWorld.BuyItem(sessionId, p->ItemType, p->Quantity);
}

void PacketManager::HandleShopSellItemPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<SellItemPacket*>(packet);
	_gameWorld.SellItem(sessionId, p->ItemID);
}

void PacketManager::HandleRequestQuestPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<RequestQuestPacket*>(packet);
	_gameWorld.RequestQuest(sessionId, p->Quest);
}

void PacketManager::HandleCompleteQuestPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<CompleteQuestPacket*>(packet);
	_gameWorld.CompleteQuest(sessionId, p->Quest);
}

void PacketManager::HandleRejectQuestPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<RejectQuestPacket*>(packet);
	_gameWorld.RejectQuest(sessionId, p->Quest);
}

void PacketManager::HandleChatSendPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<ChatSendPacket*>(packet);
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsInGame()) return;

	auto player = session->GetPlayer();
	if (!player) return;

	const char* nickname = player->GetInfo().NickName;
	auto channel = p->Channel;
	switch (channel)
	{
	case EChatChannel::All:
	{
		ChatBroadcastPacket broadcastPkt(nickname, p->Message, channel);
		_sessionMgr.BroadcastToAll(&broadcastPkt);
		break;
	}
	case EChatChannel::Friend:
	{
		ChatBroadcastPacket broadcastPkt(nickname, p->Message, channel);
		_sessionMgr.SendPacket(sessionId, &broadcastPkt);
		_sessionMgr.BroadcastToFriends(sessionId, &broadcastPkt);
		break;
	}
	case EChatChannel::Zone:
	{
		ZoneType zone = player->GetZone();
		ChatBroadcastPacket broadcastPkt(nickname, p->Message, channel);
		_gameWorld.BroadcastToZone(zone, &broadcastPkt);
		break;
	}
	default:
		break;
	}
}

void PacketManager::HandleChatWhisperPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<ChatWhisperPacket*>(packet);
	auto sender = _sessionMgr.GetSession(sessionId);
	if (!sender || !sender->IsInGame()) return;

	if (sender->GetNickName() == p->TargetNickName)
	{
		FriendOperationResultPacket resPkt(EFriendOpType::Request, ResultCode::SELF_REQUEST);
		_sessionMgr.SendPacket(sessionId, &resPkt);
		return;
	}

	int32 targetId = _sessionMgr.FindSessionIdByName(p->TargetNickName);
	auto receiver = _sessionMgr.GetSession(targetId);
	if (!receiver || !receiver->IsInGame())
	{
		FriendOperationResultPacket resPkt(EFriendOpType::Request, ResultCode::TARGET_NOT_IN_GAME);
		_sessionMgr.SendPacket(sessionId, &resPkt);
		return;
	}

	EChatChannel channel = EChatChannel::Whisper;
	auto senderName = sender->GetPlayerInfo().NickName;
	ChatBroadcastPacket pkt(senderName, p->Message, channel);

	_sessionMgr.SendPacket(sessionId, &pkt);
	_sessionMgr.SendPacket(targetId, &pkt);
}


void PacketManager::HandleFriendAddRequestPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<FriendAddRequestPacket*>(packet);
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsLogin()) return;

	auto myId = session->GetDBID();
	auto targetNick = ConvertToWString(p->TargetNickName);
	auto targetId = DBManager::GetInst().FindUserDBId(targetNick);
	ResultCode resCode = DBManager::GetInst().AddFriendRequest(myId, targetId);

	FriendOperationResultPacket resPkt(EFriendOpType::Request, resCode);
	_sessionMgr.SendPacket(sessionId, &resPkt);
	if (resCode != ResultCode::SUCCESS) return;

	int32 targetSessId = _sessionMgr.GetOnlineSessionIdByDBId(targetId);
	if (targetSessId != -1)
	{
		auto& pInfo = session->GetPlayerInfo();
		auto name = pInfo.GetName();
		FFriendInfo info;
		info.DBId = myId;
		info.SetName(ConvertToWString(name));
		info.Level = pInfo.GetLevel();
		info.isOnline = true;
		FriendRequestPacket requestPkt(info);
		_sessionMgr.SendPacket(targetSessId, &requestPkt);
	}
}

void PacketManager::HandleFriendRemoveRequestPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<FriendRemoveRequestPacket*>(packet);
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsLogin()) return;

	auto myId = session->GetDBID();
	auto targetId = p->TargetDBID;

	auto ret = DBManager::GetInst().RemoveFriend(myId, targetId);

	FriendOperationResultPacket resPkt(EFriendOpType::Remove, ret);
	_sessionMgr.SendPacket(sessionId, &resPkt);

	if (ret != ResultCode::SUCCESS) return;
	session->RemoveFriend(targetId);

	RemoveFriendPacket myPkt(targetId);
	_sessionMgr.SendPacket(sessionId, &myPkt);
	int32 targetSessId = _sessionMgr.GetOnlineSessionIdByDBId(targetId);
	if (targetSessId != -1)
	{
		auto targetSession = _sessionMgr.GetSession(targetSessId);
		targetSession->RemoveFriend(myId);

		RemoveFriendPacket targetPkt(myId);
		_sessionMgr.SendPacket(targetSessId, &targetPkt);
	}
}

void PacketManager::HandleFriendAcceptRequestPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<FriendAcceptRequestPacket*>(packet);
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsLogin()) return;

	auto myId = session->GetDBID();
	auto targetId = p->TargetDBID;

	auto ret = DBManager::GetInst().AcceptFriendRequest(myId, targetId);
	ResultCode code = ret.first;
	std::optional<FFriendInfo> friendInfoOpt = ret.second;

	FriendOperationResultPacket resPkt(EFriendOpType::Accept, code);
	_sessionMgr.SendPacket(sessionId, &resPkt);

	if (code != ResultCode::SUCCESS || !friendInfoOpt.has_value())
		return;

	FFriendInfo friendInfo = friendInfoOpt.value();
	friendInfo.bAccepted = true;
	session->AddFriend(friendInfo);

	AddFriendPacket myPkt(friendInfo);
	_sessionMgr.SendPacket(sessionId, &myPkt);

	int32 targetSessId = _sessionMgr.GetOnlineSessionIdByDBId(targetId);
	if (targetSessId != -1)
	{
		auto targetSession = _sessionMgr.GetSession(targetSessId);
		if (targetSession)
		{
			auto& selfInfo = session->GetPlayerInfo();

			FFriendInfo myInfo;
			myInfo.DBId = myId;
			myInfo.SetName(ConvertToWString(selfInfo.GetName()));
			myInfo.Level = selfInfo.Stats.Level;
			myInfo.bAccepted = true;

			targetSession->AddFriend(myInfo);

			AddFriendPacket targetPkt(myInfo);
			_sessionMgr.SendPacket(targetSessId, &targetPkt);
		}
	}
}


void PacketManager::HandleFriendRejectRequestPacket(int32 sessionId, Packet* packet)
{
	auto* p = static_cast<FriendRejectRequestPacket*>(packet);
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsLogin()) return;

	auto myId = session->GetDBID();
	auto targetId = p->TargetDBID;

	auto ret = DBManager::GetInst().RejectFriendRequest(myId, targetId);
	FriendOperationResultPacket resPkt(EFriendOpType::Reject, ret);
	_sessionMgr.SendPacket(sessionId, &resPkt);
}
