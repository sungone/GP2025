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
	case EPacketType::C_MOVE:
		LOG(LogType::RecvLog, std::format("MovePacket from [{}]", sessionId));
		HandleMovePacket(sessionId, packet);
		break;
	case EPacketType::C_ATTACK:
		LOG(LogType::RecvLog, std::format("AttackPacket from [{}]", sessionId));
		HandleAttackPacket(sessionId, packet);
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
	default:
		LOG(LogType::RecvLog, "Unknown Packet Type");
	}
}

void PacketManager::HandleSignUpPacket(int32 sessionId, Packet* packet)
{
	auto pkt = static_cast<SignUpPacket*>(packet);
#ifdef DB_LOCAL
	auto res = _dbMgr.SignUpUser(pkt->AccountID, pkt->AccountPW, ConvertToWString(pkt->NickName));
	if (res.code == DBResultCode::SUCCESS)
	{
		LOG(LogType::Log, std::format("SignUp Success [{}] userId: {}", sessionId, res.dbId));
		_sessionMgr.HandleLogin(sessionId);
		auto& playerInfo = _gameWorld.GetInfo(sessionId);
		playerInfo.SetName(pkt->NickName);
		SignUpSuccessPacket spkt(playerInfo);
		_sessionMgr.SendPacket(sessionId, &spkt);
		return;
	}
	else if (res.code == DBResultCode::DUPLICATE_ID)
		LOG(LogType::Log, "SignUp Failed! Duplicate ID");
	else
		LOG(LogType::Log, "SignUp Failed!");

	SignUpFailPacket failpkt(res.code);
	_sessionMgr.SendPacket(sessionId, &failpkt);
#endif
}

void PacketManager::HandleLoginPacket(int32 sessionId, Packet* packet)
{
	auto pkt = static_cast<LoginPacket*>(packet);
#ifdef DB_LOCAL
	auto res = _dbMgr.CheckLogin(pkt->AccountID, pkt->AccountPW);

	if (res.code == DBResultCode::SUCCESS)
	{
		LOG(LogType::Log, std::format("Login Success [{}] userId: {}, nickname: {}", sessionId, res.dbId, res.nickname));
		_sessionMgr.HandleLogin(sessionId);
		auto& playerInfo = _gameWorld.GetInfo(sessionId);
		playerInfo.SetName(res.nickname.c_str());
		LoginSuccessPacket loginpkt(playerInfo);
		_sessionMgr.SendPacket(sessionId, &loginpkt);
		return;
	}
	else if (res.code == DBResultCode::INVALID_USER)
		LOG(LogType::Log, "Login Failed! Invalid User");
	else if (res.code == DBResultCode::INVALID_PASSWORD)
		LOG(LogType::Log, "Login Failed! Invalid Password");
	else
		LOG(LogType::Log, "Login Failed! DB Error");

	LoginFailPacket failpkt(res.code);
	_sessionMgr.SendPacket(sessionId, &failpkt);
#else
	LOG(std::format("ID: {}, PW: {}", pkt->AccountID, pkt->AccountPW));
	_sessionMgr.HandleLogin(sessionId);
	auto& playerInfo = _gameWorld.GetInfo(sessionId);
	LoginSuccessPacket loginpkt(playerInfo);
	_sessionMgr.SendPacket(sessionId, &loginpkt);
#endif
}

void PacketManager::HandleLogoutPacket(int32 sessionId)
{
	auto pkt = IDPacket(EPacketType::S_REMOVE_PLAYER, sessionId);
	//Todo: 로그인 전에 종료되면 여기서  "Invalid" 호출되는 것 같은데 해결하자
	_sessionMgr.BroadcastToViewList(&pkt, sessionId);
	_sessionMgr.Disconnect(sessionId);
}

void PacketManager::HandleMovePacket(int32 sessionId, Packet* packet)
{
	MovePacket* p = static_cast<MovePacket*>(packet);
	_gameWorld.PlayerMove(p->PlayerID, p->PlayerPos, p->State,p->MoveTime);
}

void PacketManager::HandleAttackPacket(int32 sessionId, Packet* packet)
{
	AttackPacket* p = static_cast<AttackPacket*>(packet);
	_gameWorld.PlayerAttack(sessionId, p->PlayerYaw);
}

void PacketManager::HandleUseSkillPacket(int32 sessionId, Packet* packet)
{
	UseSkillPacket* p = static_cast<UseSkillPacket*>(packet);
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