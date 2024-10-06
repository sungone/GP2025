#include "Define.h"
#include "Session.h"

extern std::array<Session, MAX_CLIENT> clients;

void Session::process_packet(int32 id, uint8_t* packet)
{
	uint8_t packet_type = packet[0];
	switch (static_cast<EPacketType>(packet_type))
	{
	case EPacketType::C_LOGIN:
		std::cout << "Login Packet[" << id << "]" << std::endl;
		clients[id].is_login = true;
		clients[id].send_login_packet();
		for (auto& cl : clients)
		{
			if (not cl.is_login) continue;
			if (cl.id == id) continue;
			cl.send_add_player_packet(id);
		}


		break;
	case EPacketType::C_LOGINOUT:
		std::cout << "LoginOut Packet[" << id << "]" << std::endl;
		break;
	case EPacketType::C_MOVE:
	{
		FMovePacket* p = reinterpret_cast<FMovePacket*>(packet);
		memcpy(&clients[id].pos, &(p->VecInfo), sizeof(FVectorInfo));
		std::cout << "Move Packet[" << id << "] ("
			<< clients[id].pos.X << ", "
			<< clients[id].pos.Y << ", "
			<< clients[id].pos.Z << ")" 
			<< std::endl;
		for (auto& cl : clients)
			if (cl.is_login)
				cl.send_move_packet(id);

		break;
	}
	}
}

void Session::send_move_packet(int32 id)
{
	FMovePacket pk;
	pk.Header.PacketSize = sizeof(FMovePacket);
	pk.Header.PacketType = EPacketType::S_MOVE_PLAYER;
	pk.PlayerID = id;
	pk.VecInfo = clients[id].pos;
	do_send(&pk);
}

void Session::send_login_packet()
{
	FLoginInfoPacket pk;
	pk.Header.PacketSize = sizeof(FLoginInfoPacket);
	pk.Header.PacketType = EPacketType::S_LOGININFO;
	pk.PlayerID = id;
	pk.VecInfo = pos;
	do_send(&pk);
}

void Session::send_add_player_packet(int32 id)
{
	FLoginInfoPacket pk;
	pk.Header.PacketSize = sizeof(FLoginInfoPacket);
	pk.Header.PacketType = EPacketType::S_LOGININFO;
	pk.PlayerID = id;
	pk.VecInfo = pos;
	do_send(&pk);
}
