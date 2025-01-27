#include "PacketManager.h"

void PacketManager::ProcessPacket(Session& session, char* packet)
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
	default:
		LOG(LogType::RecvLog, "Unknown Packet Type");
	}
}

void PacketManager::HandleLoginPacket(Session& session)
{
	LOG(LogType::RecvLog, std::format("Login PKT [{}]", session.id));
	auto& playerInfo = session.player.get()->GetInfo();
	session.Login();
	auto loginPkt = InfoPacket(EPacketType::S_LOGIN_SUCCESS, playerInfo);
	session.DoSend(&loginPkt);

	auto myInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, playerInfo);
	sessionMgr.Broadcast(&myInfoPkt, session.id);

	for (auto& cl : sessions)
	{
		if (cl.id == session.id || !cl.bLogin)
			continue;
		auto otherInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, cl.player->GetInfo());
		session.DoSend(&otherInfoPkt);
	}
	gameMgr.SpawnMonster(session);
}

void PacketManager::HandleLogoutPacket(Session& session)
{
	LOG(LogType::RecvLog, std::format("Logout PKT [{}]", session.id));
	session.Disconnect();
	auto pkt = IDPacket(EPacketType::S_REMOVE_PLAYER, session.id);
	sessionMgr.Broadcast(&pkt, session.id);
}

void PacketManager::HandleMovePacket(Session& session, char* packet)
{
	auto& playerInfo = session.player.get()->GetInfo();
	LOG(LogType::RecvLog, std::format("Move PKT [{}] ({}, {}, {} / Yaw: {}, State {})",
		playerInfo.ID, playerInfo.X, playerInfo.Y, playerInfo.Z, playerInfo.Yaw, playerInfo.State));

	InfoPacket* p = reinterpret_cast<InfoPacket*>(packet);
	playerInfo = p->Data;
	auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, playerInfo);
	sessionMgr.Broadcast(&pkt, session.id);
}

void PacketManager::HandleAttackPacket(Session& session, char* packet)
{
	LOG(LogType::RecvLog, std::format("Attack PKT [{}]", session.id));
	
	auto& playerInfo = session.player.get()->GetInfo();
	uint8_t packetSize = static_cast<EPacketType>(packet[PKT_SIZE_INDEX]);
	if (packetSize == sizeof(InfoPacket))
	{
		InfoPacket* p = reinterpret_cast<InfoPacket*>(packet);
		playerInfo = p->Data;
		auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, playerInfo);
		sessionMgr.Broadcast(&pkt, session.id);
	}
	else
	{
		//todo: 충돌처리 서버에서 하도록

		AttackPacket* p = reinterpret_cast<AttackPacket*>(packet);
		FAttackData data = p->Data;
		LOG(LogType::Log, std::format("Attacker[{}] -> Attacked[{}]", data.Attacker.ID, data.Attacked.ID));

		// for anim 동기화
		playerInfo = data.Attacker;
		auto pkt1 = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, playerInfo);
		sessionMgr.Broadcast(&pkt1);

		// hp 감소
		gameMgr.OnDamaged(playerInfo.Damage, data.Attacked);
		auto monster = gameMgr.GetInfo(data.Attacked.ID);
		auto pkt2 = InfoPacket(EPacketType::S_MONSTER_STATUS_UPDATE, monster);
		sessionMgr.Broadcast(&pkt2);
	}
}
