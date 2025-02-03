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
	default:
		LOG(LogType::RecvLog, "Unknown Packet Type");
	}
}

void PacketManager::HandleLoginPacket(Session& session)
{
	auto playerInfo = session.GetPlayerInfo();
	int32 id = playerInfo.ID;
	LOG(LogType::RecvLog, std::format("Login PKT [{}]", id));

	session.SetLogin();
	auto loginPkt = InfoPacket(EPacketType::S_LOGIN_SUCCESS, playerInfo);
	session.DoSend(&loginPkt);

	auto myInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, playerInfo);
	_sessionMgr.Broadcast(&myInfoPkt, id);

	for (auto& cl : _sessions)
	{
		if (cl.GetId() == id || !cl.IsLogin())
			continue;
		auto otherInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, cl.GetPlayerInfo());
		session.DoSend(&otherInfoPkt);
	}
	_gameMgr.SpawnMonster(session);
}

void PacketManager::HandleLogoutPacket(Session& session)
{
	int32 id = session.GetId();
	LOG(LogType::RecvLog, std::format("Logout PKT [{}]", id));
	session.Disconnect();
	auto pkt = IDPacket(EPacketType::S_REMOVE_PLAYER, id);
	_sessionMgr.Broadcast(&pkt, id);
}

void PacketManager::HandleMovePacket(Session& session, BYTE* packet)
{
	auto& playerInfo = session.GetPlayerInfo();
	int32 id = playerInfo.ID;
	LOG(LogType::RecvLog, std::format("Move PKT [{}] ({:.2f}, {:.2f}, {:.2f} / Yaw: {:.2f}, State {})",
		playerInfo.ID, playerInfo.X, playerInfo.Y, playerInfo.Z, playerInfo.Yaw, playerInfo.State));

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

	uint8_t packetSize = static_cast<EPacketType>(packet[PKT_SIZE_INDEX]);
	if (packetSize == sizeof(InfoPacket))
	{
		InfoPacket* p = reinterpret_cast<InfoPacket*>(packet);
		playerInfo = p->Data;
		auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, playerInfo);
		_sessionMgr.Broadcast(&pkt, id);
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
		_sessionMgr.Broadcast(&pkt1);

		// hp 감소
		float BaseDamage = playerInfo.Damage;
		float CrtRate = playerInfo.CrtRate;
		float CrtValue = playerInfo.CrtValue;

		static std::default_random_engine dre;
		static std::uniform_real_distribution<float>  dist(0.f, 1.f);
		float RandomValue = dist(dre);

		bool bIsCritical = RandomValue < CrtRate;

		float FinalDamage = bIsCritical ? BaseDamage * CrtValue : BaseDamage;

		if(_gameMgr.OnDamaged(playerInfo.Damage, data.Attacked))
		{
			auto monster = _gameMgr.GetInfo(data.Attacked.ID);
			auto pkt2 = InfoPacket(EPacketType::S_MONSTER_STATUS_UPDATE, monster);
			_sessionMgr.Broadcast(&pkt2);
		}
	}
}
