#include "pch.h"
#include "PacketManager.h"

void PacketManager::ProcessPacket(int32 sessionId, Packet* packet)
{
	EPacketType packetType = static_cast<EPacketType>(packet->Header.PacketType);

	switch (packetType)
	{
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

void PacketManager::HandleLoginPacket(int32 sessionId, Packet* packet)
{
	auto pkt = static_cast<LoginPacket*>(packet);
	LOG(std::format("ID: {}, PW: {}", pkt->AccountID, pkt->AccountPW));
	_sessionMgr.HandleLogin(sessionId);
}

void PacketManager::HandleLogoutPacket(int32 sessionId)
{
	auto pkt = IDPacket(EPacketType::S_REMOVE_PLAYER, sessionId);
	_sessionMgr.BroadcastToViewList(&pkt, sessionId);
	_sessionMgr.Disconnect(sessionId);
}

void PacketManager::HandleMovePacket(int32 sessionId, Packet* packet)
{
	InfoPacket* p = static_cast<InfoPacket*>(packet);
	auto& playerInfo = p->Data;
	_gameWorld.PlayerMove(sessionId, p->Data);
}

void PacketManager::HandleAttackPacket(int32 sessionId, Packet* packet)
{
	AttackPacket* p = static_cast<AttackPacket*>(packet);
	_gameWorld.PlayerAttack(sessionId);
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