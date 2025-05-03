#include "pch.h"
#include "PacketManager.h"

void PacketManager::ProcessPacket(int32 sessionId, Packet* packet)
{
	EPacketType packetType = static_cast<EPacketType>(packet->Header.PacketType);

	switch (packetType)
	{
	case EPacketType::C_SIGNUP:
		LOG(LogType::RecvLog, std::format("SignUpPacket from [{}]", sessionId));
		HandleSignUpPacket(sessionId, packet);
		break;
	case EPacketType::C_LOGIN:
		LOG(LogType::RecvLog, std::format("LoginPacket from [{}]", sessionId));
		HandleLoginPacket(sessionId, packet);
		break;
	case EPacketType::C_LOGOUT:
		LOG(LogType::RecvLog, std::format("LogoutPacket from [{}]", sessionId));
		HandleLogoutPacket(sessionId);
		break;
	case EPacketType::C_SELECT_CHARACTER:
		LOG(LogType::RecvLog, std::format("SelectCharacter from [{}]", sessionId));
		HandleSelectCharacterPacket(sessionId, packet);
		break;
	case EPacketType::C_ENTER_GAME:
		LOG(LogType::RecvLog, std::format("EnterGamePacket from [{}]", sessionId));
		HandleEnterGamePacket(sessionId, packet);
		break;

	case EPacketType::C_MOVE:
		LOG(LogType::RecvLog, std::format("MovePacket from [{}]", sessionId));
		HandleMovePacket(sessionId, packet);
		break;
	case EPacketType::C_ATTACK:
		LOG(LogType::RecvLog, std::format("AttackPacket from [{}]", sessionId));
		HandleAttackPacket(sessionId, packet);
		break;
	case EPacketType::C_START_AIMING:
		LOG(LogType::RecvLog, std::format("StartAimingPacket from [{}]", sessionId));
		HandleStartAimingPacket(sessionId, packet);
		break;
	case EPacketType::C_STOP_AIMING:
		LOG(LogType::RecvLog, std::format("StopAimingPacket from [{}]", sessionId));
		HandleStopAimingPacket(sessionId, packet);
		break;

	case EPacketType::C_USE_SKILL:
		LOG(LogType::RecvLog, std::format("UseSkillPacket from [{}]", sessionId));
		HandleUseSkillPacket(sessionId, packet);
		break;


	case EPacketType::C_TAKE_ITEM:
		LOG(LogType::RecvLog, std::format("PickUpItemPacket from [{}]", sessionId));
		HandlePickUpItemPacket(sessionId, packet);
		break;
	case EPacketType::C_DROP_ITEM:
		LOG(LogType::RecvLog, std::format("DropItemPacket from [{}]", sessionId));
		HandleDropItemPacket(sessionId, packet);
		break;
	case EPacketType::C_USE_ITEM:
		LOG(LogType::RecvLog, std::format("UseItemPacket from [{}]", sessionId));
		HandleUseItemPacket(sessionId, packet);
		break;
	case EPacketType::C_EQUIP_ITEM:
		LOG(LogType::RecvLog, std::format("EquipItemPacket from [{}]", sessionId));
		HandleEquipItemPacket(sessionId, packet);
		break;
	case EPacketType::C_UNEQUIP_ITEM:
		LOG(LogType::RecvLog, std::format("UnequipItemPacket from [{}]", sessionId));
		HandleUnequipItemPacket(sessionId, packet);
		break;

	case EPacketType::C_CHANGE_ZONE:
		LOG(LogType::RecvLog, std::format("RequestZoneChange from [{}]", sessionId));
		HandleZoneChangeRequestPacket(sessionId, packet);
		break;
	case EPacketType::C_RESPAWN:
		LOG(LogType::RecvLog, std::format("RespawnRequest from [{}]", sessionId));
		HandleRespawnRequestPacket(sessionId, packet);
		break;

	default:
		LOG(LogType::RecvLog, "Unknown Packet Type");
	}
}

void PacketManager::HandleSignUpPacket(int32 sessionId, Packet* packet)
{
#ifdef DB_LOCAL
	auto pkt = static_cast<SignUpPacket*>(packet);
	std::wstring name = ConvertToWString(pkt->NickName);
	auto res = _dbMgr.SignUpUser(sessionId, pkt->AccountID, pkt->AccountPW, name);
	if (res.code != DBResultCode::SUCCESS)
	{
		LOG(std::format("SignUp Failed [{}]", sessionId));
		SignUpFailPacket failpkt(res.code);
		_sessionMgr.SendPacket(sessionId, &failpkt);
		return;
	}
	_sessionMgr.HandleLogin(sessionId, res);
	auto& playerInfo = _gameWorld.GetInfo(sessionId);
	SignUpSuccessPacket spkt;
	_sessionMgr.SendPacket(sessionId, &spkt);
	LOG(std::format("SignUp Success [{}] userId: {}", sessionId, res.dbId));
	return;
#endif
}

void PacketManager::HandleLoginPacket(int32 sessionId, Packet* packet)
{
	auto pkt = static_cast<LoginPacket*>(packet);
#ifdef DB_LOCAL
	auto res = _dbMgr.CheckLogin(sessionId, pkt->AccountID, pkt->AccountPW);

	if (res.code != DBResultCode::SUCCESS)
	{
		LoginFailPacket failpkt(res.code);
		_sessionMgr.SendPacket(sessionId, &failpkt);
		return;
	}
	LoginSuccessPacket loginpkt;
	_sessionMgr.SendPacket(sessionId, &loginpkt);
	_sessionMgr.HandleLogin(sessionId, res);

	LOG(LogType::Log, std::format("Login Success [{}] userId: {}", sessionId, res.dbId));
	return;

#else
	LOG(std::format("ID: {}, PW: {}", pkt->AccountID, pkt->AccountPW));
	_sessionMgr.HandleLogin(sessionId);
	auto& playerInfo = _gameWorld.GetInfo(sessionId);
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
	if(newType != Type::EPlayer::NONE)
		player->SetCharacterType(p->PlayerType);

	session->EnterGame();
	_gameWorld.PlayerEnterGame(player);
	auto& playerInfo = _gameWorld.GetInfo(sessionId);
	EnterGamePacket enterpkt(playerInfo);
	_sessionMgr.SendPacket(sessionId, &enterpkt);

	//실패처리를 따로 할까나말까나..
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
	_gameWorld.PlayerSetYaw(sessionId, p->PlayerYaw);
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
	_gameWorld.PlayerSetYaw(sessionId, p->PlayerYaw);
	_gameWorld.PlayerAddState(sessionId, ECharacterStateType::STATE_AUTOATTACK);
	_gameWorld.PlayerAttack(sessionId);
}

void PacketManager::HandleUseSkillPacket(int32 sessionId, Packet* packet)
{
	UseSkillPacket* p = static_cast<UseSkillPacket*>(packet);
	_gameWorld.PlayerSetYaw(sessionId, p->PlayerYaw);
	_gameWorld.PlayerUseSkill(sessionId, p->SkillGID);
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
	_gameWorld.DropInventoryItem(sessionId, itemid);
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
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsInGame()) return;

	auto player = session->GetPlayer();
	if (!player) return;

	auto* p = static_cast<RequestZoneChangePacket*>(packet);
	ZoneType targetZone = p->TargetZone;

	FVector newPos = _gameWorld.TransferToZone(sessionId, targetZone); // 새 위치 반환한다고 가정

	ChangeZonePacket response(targetZone, newPos);
	_sessionMgr.SendPacket(sessionId, &response);
}

void PacketManager::HandleRespawnRequestPacket(int32 sessionId, Packet* packet)
{
	auto session = _sessionMgr.GetSession(sessionId);
	if (!session || !session->IsInGame()) return;

	auto player = session->GetPlayer();
	if (!player) return;

	auto* p = static_cast<RespawnRequestPacket*>(packet);
	ZoneType targetZone = p->TargetZone;

	FVector respawnPos = _gameWorld.RespawnPlayer(sessionId, targetZone);

	auto& info = _gameWorld.GetInfo(sessionId);
	RespawnPacket response(info);
	_sessionMgr.SendPacket(sessionId, &response);
}
