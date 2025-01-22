#include "PacketManager.h"

void PacketManager::ProcessPacket(Session& session, char* packet)
{
	EPacketType packetType = static_cast<EPacketType>(packet[0]);

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
	sessionMgr.Broadcast(&myInfoPkt,session.id);

	for (auto& cl : sessions)
	{
		if (cl.id == session.id || !cl.id)
			continue;
		auto otherInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, cl.info);
		session.DoSend(&otherInfoPkt);
	}

}

void PacketManager::HandleLogoutPacket(Session& session)
{
	session.Disconnect();
	auto pkt = IDPacket(EPacketType::S_REMOVE_PLAYER, session.id);
	sessionMgr.Broadcast(&pkt,session.id);
}

void PacketManager::HandleMovePacket(Session& session, char* packet)
{
	InfoPacket* p = reinterpret_cast<InfoPacket*>(packet);
	session.info = p->Data;
	auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, session.info);
	sessionMgr.Broadcast(&pkt,session.id);
}

void PacketManager::HandleAttackPacket(Session& session, char* packet)
{
	//todo:
	// - 내 정보, 공격 대상 정보 -> 업데이트&전송

	InfoPacket* p = reinterpret_cast<InfoPacket*>(packet);
	session.info = p->Data;
	auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, session.info);
	sessionMgr.Broadcast(&pkt, session.id);
}
