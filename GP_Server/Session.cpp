#include "Define.h"
#include "Session.h"

extern std::array<Session, MAX_CLIENT> clients;

void Session::process_packet(char* packet)
{
	int32 recv_id = getId();
	EPacketType packet_type = static_cast<EPacketType>(packet[0]);
	switch (packet_type)
	{
	case EPacketType::C_LOGIN:
		std::cout << "<- Recv:: Login Packet[" << recv_id << "]" << std::endl;
		clients[recv_id].is_login = true;
		clients[recv_id].send_login_packet();
		for (auto& cl : clients)
		{
			if (cl.getId() == recv_id) continue;
			if (not cl.is_login) continue;
			cl.send_add_player_packet(recv_id);
		}
		for (auto& cl : clients)
		{
			if (cl.getId() == recv_id) continue;
			if (not cl.is_login) continue;
			send_add_player_packet(cl.getId());
		}
		break;
	case EPacketType::C_LOGOUT:
		std::cout << "<- Recv:: LoginOut Packet[" << recv_id << "]" << std::endl;
		disconnect();
		break;
	case EPacketType::C_MOVE:
	{
		FMovePacket* p = reinterpret_cast<FMovePacket*>(packet);
		memcpy(&clients[recv_id].info, &(p->PlayerInfo), sizeof(FPlayerInfo));
		std::cout << "<- Recv:: Move Packet[" << recv_id << "] ("
			<< clients[recv_id].info.X << ", "
			<< clients[recv_id].info.Y << ", "
			<< clients[recv_id].info.Z << "), "
			<< "State " << clients[recv_id].info.State << ""
			<< std::endl;
		for (auto& cl : clients)
		{
			if (cl.getId() == recv_id) continue;
			if (cl.is_login)
				cl.send_move_packet(recv_id);
		}
		break;
	}
	default:
		std::cout << "<- Recv:: Unknown Packet" << std::endl;
	}
}

void Session::send_move_packet(int32 id)
{
	FMovePacket pk;
	pk.Header.PacketSize = sizeof(FMovePacket);
	pk.Header.PacketType = EPacketType::S_MOVE_PLAYER;
	pk.PlayerInfo = clients[id].info;
	std::cout << "-> Send:: Move Packet [" << id << "] ("
		<< pk.PlayerInfo.X << ","
		<< pk.PlayerInfo.Y << ","
		<< pk.PlayerInfo.Z << ") to [" << this->getId() << "]"
		<< std::endl;
	do_send(&pk);
}

void Session::send_login_packet()
{
	FLoginInfoPacket pk;
	pk.Header.PacketSize = sizeof(FLoginInfoPacket);
	pk.Header.PacketType = EPacketType::S_LOGININFO;
	pk.PlayerInfo = info;
	std::cout << "-> Send:: Login Info Packet [" << pk.PlayerInfo.ID << "]" << std::endl;
	do_send(&pk);
}

void Session::send_add_player_packet(int32 add_id)
{
	FAddPlayerPacket pk;
	pk.Header.PacketSize = sizeof(FAddPlayerPacket);
	pk.Header.PacketType = EPacketType::S_ADD_PLAYER;
	pk.PlayerID = this->getId();
	pk.PlayerInfo = clients[add_id].info;
	std::cout << "-> Send:: Add Player[" << pk.PlayerInfo.ID << "] Packet to [" << pk.PlayerID << "]" << std::endl;
	do_send(&pk);
}

void Session::send_remove_player_packet(int32 remove_id)
{
	FRemovePlayerPacket pk;
	pk.Header.PacketSize = sizeof(FRemovePlayerPacket);
	pk.Header.PacketType = EPacketType::S_REMOVE_PLAYER;
	pk.PlayerID = remove_id;
	std::cout << "-> Send:: Remove Player[" << pk.PlayerID << "] Packet to [" << this->getId() << "]" << std::endl;
	do_send(&pk);
}

void Session::disconnect()
{
	this->is_login = false;
	for (auto& cl : clients)
		if (cl.is_login)
			clients[cl.getId()].send_remove_player_packet(this->getId());
}
