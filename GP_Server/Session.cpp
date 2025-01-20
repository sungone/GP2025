#include "Session.h"
#include "SessionManager.h"

//Todo : ∏Æ∆—≈‰∏µ
std::array<Session, MAX_CLIENT>& clients  = SessionManager::GetInst().clients;

void Session::process_packet(char* packet)
{
	int32 recv_id = getId();
	EPacketType packet_type = static_cast<EPacketType>(packet[0]);
	switch (packet_type)
	{
	case EPacketType::C_LOGIN:
		LOG(LogType::RecvLog, std::format("Login PKT [{}]", recv_id));
		clients[recv_id].Login();
		clients[recv_id].send_login_packet();
		for (auto& cl : clients)
		{
			if (cl.getId() == recv_id) continue;
			if (not cl.bLogin) continue;
			cl.send_add_player_packet(recv_id);
		}
		for (auto& cl : clients)
		{
			if (cl.getId() == recv_id) continue;
			if (not cl.bLogin) continue;
			send_add_player_packet(cl.getId());
		}
		break;
	case EPacketType::C_LOGOUT:
		LOG(LogType::RecvLog, std::format("LoginOut PKT [{}]", recv_id));
		Disconnect();
		break;
	case EPacketType::C_MOVE:
	{
		FMovePacket* p = reinterpret_cast<FMovePacket*>(packet);
		memcpy(&clients[recv_id].info, &(p->PlayerInfo), sizeof(FPlayerInfo));
		LOG(LogType::RecvLog, std::format("Move PKT [{}] ({}, {}, {} / Yaw: {}), State {} ",
			recv_id, clients[recv_id].info.X, clients[recv_id].info.Y, clients[recv_id].info.Z,
			clients[recv_id].info.Yaw, clients[recv_id].info.State));

		for (auto& cl : clients)
		{
			if (cl.getId() == recv_id) continue;
			if (cl.bLogin)
				cl.send_move_packet(recv_id);
		}
		break;
	}
	case EPacketType::C_ATTACK:
	{
		FAttackPacket* p = reinterpret_cast<FAttackPacket*>(packet);
		memcpy(&clients[recv_id].info, &(p->PlayerInfo), sizeof(FPlayerInfo));
		LOG(LogType::RecvLog, std::format("Attack PKT [{}]", recv_id));

		for (auto& cl : clients)
		{
			if (cl.getId() == recv_id) continue;
			if (cl.bLogin)
				cl.send_attack_packet(recv_id);
		}
		break;
	}
	default:
		LOG(LogType::SendLog, std::format("Unknown Packet"));
	}
}

void Session::send_move_packet(int32 id)
{
	auto pk = FObjectInfoPacket(EPacketType::S_MOVE_PLAYER, clients[id].info);
	LOG(LogType::SendLog, std::format("Move PKT [{}]", this->getId()));

	DoSend(&pk);
}

void Session::send_attack_packet(int32 id)
{
	auto pk = FObjectInfoPacket(EPacketType::S_ATTACK_PLAYER, clients[id].info);
	LOG(LogType::SendLog, std::format("Attack PKT [{}]", this->getId()));

	DoSend(&pk);
}

void Session::send_login_packet()
{
	auto pk = FObjectInfoPacket(EPacketType::S_LOGININFO, info);
	LOG(LogType::SendLog, std::format("LoginInfo PKT [{}]", pk.Info.ID));
	DoSend(&pk);
}

void Session::send_add_player_packet(int32 add_id)
{
	FAddPlayerPacket pk;
	pk.Header.PacketSize = sizeof(FAddPlayerPacket);
	pk.Header.PacketType = EPacketType::S_ADD_PLAYER;
	pk.PlayerID = this->getId();
	pk.PlayerInfo = clients[add_id].info;
	LOG(LogType::SendLog, std::format("AddPlayer PKT [{}] -> [{}]", pk.PlayerInfo.ID, pk.PlayerID));

	DoSend(&pk);
}

void Session::send_remove_player_packet(int32 remove_id)
{
	FRemovePlayerPacket pk;
	pk.Header.PacketSize = sizeof(FRemovePlayerPacket);
	pk.Header.PacketType = EPacketType::S_REMOVE_PLAYER;
	pk.PlayerID = remove_id;
	LOG(LogType::SendLog, std::format("RemovePlayer PKT [{}] -> [{}]", pk.PlayerID, this->getId()));

	DoSend(&pk);
}

void Session::Disconnect()
{
	this->bLogin = false;
	for (auto& cl : clients)
		if (cl.bLogin)
			clients[cl.getId()].send_remove_player_packet(this->getId());
}
