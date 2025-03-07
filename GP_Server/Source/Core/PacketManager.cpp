#include "pch.h"
#include "PacketManager.h"

void PacketManager::ProcessPacket(Session& session, BYTE* packet)
{
	EPacketType packetType = static_cast<EPacketType>(packet[PKT_TYPE_INDEX]);

	switch (packetType)
	{
	case EPacketType::C_LOGIN:
		HandleLoginPacket(session);
		break;
	case EPacketType::C_LOGOUT:
		HandleLogoutPacket(session);
		break;
	case EPacketType::C_MOVE:
		HandleMovePacket(session, packet);
		break;
	case EPacketType::C_ATTACK:
		HandleAttackPacket(session, packet);
		break;
	case EPacketType::C_TAKE_ITEM:
		HandleTakeItemPacket(session, packet);
		break;
	default:
		LOG(LogType::RecvLog, "Unknown Packet Type");
	}
}

void PacketManager::HandleLoginPacket(Session& session)
{
	session.CreatePlayer();

	auto& playerInfo = session.GetPlayerInfo();
	int32 id = playerInfo.ID;
	LOG(LogType::RecvLog, std::format("Login PKT [{}]", id));

	auto loginPkt = InfoPacket(EPacketType::S_LOGIN_SUCCESS, playerInfo);
	session.DoSend(&loginPkt);

	auto myInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, playerInfo);
	_sessionMgr.Broadcast(&myInfoPkt, id);
	_sessionMgr.HandleLogin(id);
	_gameMgr.SpawnMonster(session);
}

void PacketManager::HandleLogoutPacket(Session& session)
{
	int32 id = session.GetId();
	LOG(LogType::RecvLog, std::format("Logout PKT [{}]", id));
	
	auto pkt = IDPacket(EPacketType::S_REMOVE_PLAYER, id);
	_sessionMgr.Broadcast(&pkt, id);
	_sessionMgr.Disconnect(id);
}

void PacketManager::HandleMovePacket(Session& session, BYTE* packet)
{
	auto& playerInfo = session.GetPlayerInfo();
	int32 id = playerInfo.ID;
	LOG(LogType::RecvLog, std::format("Move PKT [{}] ({:.2f}, {:.2f}, {:.2f} / Yaw: {:.2f}, State {})",
		playerInfo.ID, playerInfo.Pos.X, playerInfo.Pos.Y, playerInfo.Pos.Z, playerInfo.Yaw, playerInfo.State));

	InfoPacket* p = reinterpret_cast<InfoPacket*>(packet);
	playerInfo = p->Data;
	auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, playerInfo);
	_sessionMgr.Broadcast(&pkt, id);
}

void PacketManager::HandleAttackPacket(Session& session, BYTE* packet)
{
	auto& playerInfo = session.GetPlayerInfo();
	int32 id = playerInfo.ID;
	LOG(LogType::RecvLog, std::format("Attack PKT [{}]", id));
	AttackPacket* p = reinterpret_cast<AttackPacket*>(packet);
	_gameMgr.ProcessAttack(id, p->TargetID);
	playerInfo.AddState(ECharacterStateType::STATE_AUTOATTACK);
	auto pkt1 = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, playerInfo);
	_sessionMgr.Broadcast(&pkt1);

}

void PacketManager::HandleTakeItemPacket(Session& session, BYTE* packet)
{
	IDPacket* p = reinterpret_cast<IDPacket*>(packet);
	auto pkt1 = ItemPkt::DespawnPacket(p->Data);
	_sessionMgr.Broadcast(&pkt1);
}
