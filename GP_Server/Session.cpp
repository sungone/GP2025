#include "Define.h"
#include "Session.h"

extern std::array<Session, MAX_CLIENT> clients;

void Session::process_packet(char* packet)
{
	EPacketType packet_type = static_cast<EPacketType>(packet[0]);
	switch (packet_type)
	{
	case EPacketType::C_LOGIN:
		std::cout << "<- Recv:: Login Packet[" << id << "]" << std::endl;
		clients[id].is_login = true;
		clients[id].send_login_packet();
		for (auto& cl : clients)
		{
			if (cl.id == id) continue;
			if (not cl.is_login) continue;
			cl.send_add_player_packet(id);
		}
		for (auto& cl : clients)
		{
			if (cl.id == id) continue;
			if (not cl.is_login) continue;
			send_add_player_packet(cl.id);
		}
		break;
	case EPacketType::C_LOGOUT:
		std::cout << "<- Recv:: LoginOut Packet[" << id << "]" << std::endl;
		disconnect();
		break;
	case EPacketType::C_MOVE:
	{
		FMovePacket* p = reinterpret_cast<FMovePacket*>(packet);
		memcpy(&clients[id].pos, &(p->VecInfo), sizeof(FVectorInfo));
		std::cout << "<- Recv:: Move Packet[" << id << "] ("
			<< clients[id].pos.X << ", "
			<< clients[id].pos.Y << ", "
			<< clients[id].pos.Z << ")"
			<< std::endl;
		for (auto& cl : clients)
		{
			if (cl.id == id) continue;
			if (cl.is_login)
				cl.send_move_packet(id);
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
	pk.PlayerID = id;
	pk.VecInfo = clients[id].pos;
	std::cout << "-> Send:: Move Packet [" << pk.PlayerID << "] ("
		<< pk.VecInfo.X << ","
		<< pk.VecInfo.Y << ","
		<< pk.VecInfo.Z << ") to[" << this->id << "]"
		<< std::endl;
	do_send(&pk);
}

void Session::send_login_packet()
{
	FLoginInfoPacket pk;
	pk.Header.PacketSize = sizeof(FLoginInfoPacket);
	pk.Header.PacketType = EPacketType::S_LOGININFO;
	pk.PlayerID = id;
	pk.VecInfo = pos;
	std::cout << "-> Send:: Login Info Packet [" << id << "]" << std::endl;
	do_send(&pk);
}

void Session::send_add_player_packet(int32 id)
{
	FAddPlayerPacket pk;
	pk.Header.PacketSize = sizeof(FAddPlayerPacket);
	pk.Header.PacketType = EPacketType::S_ADD_PLAYER;
	pk.PlayerID = id;
	pk.VecInfo = pos;
	std::cout << "-> Send:: Add Player[" << pk.PlayerID << "] Packet to [" << this->id << "]" << std::endl;
	do_send(&pk);
}

void Session::send_remove_player_packet(int32 id)
{
	FRemovePlayerPacket pk;
	pk.Header.PacketSize = sizeof(FRemovePlayerPacket);
	pk.Header.PacketType = EPacketType::S_REMOVE_PLAYER;
	pk.PlayerID = id;
	std::cout << "-> Send:: Remove Player[" << pk.PlayerID << "] Packet to [" << this->id << "]" << std::endl;
	do_send(&pk);
}

void Session::disconnect()
{
	this->is_login = false;
	for (auto& cl : clients)
		if (cl.is_login)
			clients[cl.id].send_remove_player_packet(this->id);
}
