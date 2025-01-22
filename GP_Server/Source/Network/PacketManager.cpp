#include "PacketManager.h"

void PacketManager::ProcessPacket(Session& session, char* packet)
{
	EPacketType packetType = static_cast<EPacketType>(packet[PKT_TYPE_INDEX]);

	switch (packetType)
	{
	case EPacketType::C_LOGIN:
		LOG(LogType::RecvLog, std::format("Login PKT [{}]", session.id));
		HandleLoginPacket(session);
		break;

	case EPacketType::C_LOGOUT:
		LOG(LogType::RecvLog, std::format("Logout PKT [{}]", session.id));
		HandleLogoutPacket(session);
		break;

	case EPacketType::C_MOVE:
		HandleMovePacket(session, packet);
		LOG(LogType::RecvLog, std::format("Move PKT [{}] ({}, {}, {} / Yaw: {}, State {})",
			session.id, session.info.X, session.info.Y, session.info.Z, session.info.Yaw, session.info.State));
		break;
	case EPacketType::C_ATTACK:
		LOG(LogType::RecvLog, std::format("Attack PKT [{}]", session.id));
		HandleAttackPacket(session, packet);
		break;
	default:
		LOG(LogType::RecvLog, "Unknown Packet Type");
	}
}

void PacketManager::HandleLoginPacket(Session& session)
{
	session.Login();
	auto loginPkt = InfoPacket(EPacketType::S_LOGIN_SUCCESS, session.info);
	session.DoSend(&loginPkt);

	auto myInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, session.info);
	sessionMgr.Broadcast(&myInfoPkt, session.id);

	for (auto& cl : sessions)
	{
		if (cl.id == session.id || !cl.id)
			continue;
		auto otherInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, cl.info);
		session.DoSend(&otherInfoPkt);
	}
	gameMgr.SpawnMonster(session);
}

void PacketManager::HandleLogoutPacket(Session& session)
{
	session.Disconnect();
	auto pkt = IDPacket(EPacketType::S_REMOVE_PLAYER, session.id);
	sessionMgr.Broadcast(&pkt, session.id);
}

void PacketManager::HandleMovePacket(Session& session, char* packet)
{
	InfoPacket* p = reinterpret_cast<InfoPacket*>(packet);
	session.info = p->Data;
	auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, session.info);
	sessionMgr.Broadcast(&pkt, session.id);
}

void PacketManager::HandleAttackPacket(Session& session, char* packet)
{
	uint8_t packetSize = static_cast<EPacketType>(packet[PKT_SIZE_INDEX]);
	if (packetSize == sizeof(InfoPacket))
	{
		InfoPacket* p = reinterpret_cast<InfoPacket*>(packet);
		session.info = p->Data;
		auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, session.info);
		sessionMgr.Broadcast(&pkt, session.id);
	}
	else
	{
		//todo: 충돌처리 서버에서 하도록

		AttackPacket* p = reinterpret_cast<AttackPacket*>(packet);
		FAttackData data = p->Data;
		LOG(LogType::Log, std::format("Attacker[{}] -> Attacked[{}]", data.Attacker.ID, data.Attacked.ID));

		// for anim 동기화
		session.info = data.Attacker;
		auto pkt1 = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, session.info);
		sessionMgr.Broadcast(&pkt1);

		// hp 감소
		gameMgr.OnAttackMonster(session, data.Attacked);
		auto monster = gameMgr.GetMonsterInfo(data.Attacked.ID);
		auto pkt2 = InfoPacket(EPacketType::S_MONSTER_STATUS_UPDATE, monster);
		sessionMgr.Broadcast(&pkt2);
	}
}
