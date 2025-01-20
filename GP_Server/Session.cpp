#include "Session.h"
#include <random>
#include "MonsterManager.h"

std::default_random_engine dre;
std::uniform_real_distribution<float> ud_x(-3000, -1000);
std::uniform_real_distribution<float> ud_y(-3500, -1500);
extern std::array<Session, MAX_CLIENT> clients;
extern MonsterManager MonsterMgr;

void Session::process_packet(char* packet)
{
	int32 recv_id = getId();
	EPacketType packet_type = static_cast<EPacketType>(packet[0]);
	switch (packet_type)
	{
	case EPacketType::C_LOGIN:
		std::cout << "<- Recv:: Login Packet[" << recv_id << "]" << std::endl;
		clients[recv_id].is_login = true;
		clients[recv_id].info.X = ud_x(dre);
		clients[recv_id].info.Y = ud_y(dre);
		clients[recv_id].info.Z = 116;
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

		// 클라이언트가 로그인 요청을 보내면 서버에 몬스터 한마리를 스폰시킴
		// 25.01.18 -> 이동 , 공격 빼고 스폰만 일단 해볼 예정
		{
			static int32 MonsterIDCounter = 1;

			EMonster MonsterType = EMonster::M_Mouse;

			// 몬스터 생성
			MonsterMgr.SpawnMonster(MonsterIDCounter, MonsterType, ud_x(dre), ud_y(dre), 116.f, 90.f);
			for (auto& cl : clients)
			{
				if (cl.is_login)
					cl.send_spawn_monster_packet(MonsterIDCounter);
			}

			MonsterIDCounter++;
		}

		break;
	case EPacketType::C_LOGOUT:
		std::cout << "<- Recv:: LoginOut Packet[" << recv_id << "]" << std::endl;
		disconnect();
		break;
	case EPacketType::C_MOVE:
	{
		FMovePacket* p = reinterpret_cast<FMovePacket*>(packet);
		memcpy(&clients[recv_id].info, &(p->PlayerInfo), sizeof(FCharacterInfo));

		std::cout << "<- Recv:: Move Packet[" << recv_id << "] ("
			<< clients[recv_id].info.X << ", "
			<< clients[recv_id].info.Y << ", "
			<< clients[recv_id].info.Z << "), Rotate ("
			<< clients[recv_id].info.Yaw << ") , "
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
	case EPacketType::C_ATTACK:
	{
		FAttackPacket* p = reinterpret_cast<FAttackPacket*>(packet);
		memcpy(&clients[recv_id].info, &(p->PlayerInfo), sizeof(FCharacterInfo));

		std::cout << "<- Recv:: Attack Packet[" << recv_id << "] ("
			<< clients[recv_id].info.X << ", "
			<< clients[recv_id].info.Y << ", "
			<< clients[recv_id].info.Z << "), Rotate ("
			<< clients[recv_id].info.Yaw << ") , "
			<< "State " << clients[recv_id].info.State << ""
			<< std::endl;
		for (auto& cl : clients)
		{
			if (cl.getId() == recv_id) continue;
			if (cl.is_login)
				cl.send_attack_packet(recv_id);
		}
		break;
	}
	case EPacketType::C_HIT :
	{
		std::cout << "<- Recv:: Hit Packet : Hit Character" << std::endl;
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
		<< pk.PlayerInfo.Z << ") to [" << this->getId() << "]\n";
	do_send(&pk);
}

void Session::send_attack_packet(int32 id)
{
	FAttackPacket pk;
	pk.Header.PacketSize = sizeof(FAttackPacket);
	pk.Header.PacketType = EPacketType::S_ATTACK_PLAYER;
	pk.PlayerInfo = clients[id].info;
	std::cout << "-> Send:: Attack Packet [" << id << "] ("
		<< pk.PlayerInfo.X << ","
		<< pk.PlayerInfo.Y << ","
		<< pk.PlayerInfo.Z << ") to [" << this->getId() << "]\n";
	do_send(&pk);
}

void Session::send_login_packet()
{
	FLoginInfoPacket pk;
	pk.Header.PacketSize = sizeof(FLoginInfoPacket);
	pk.Header.PacketType = EPacketType::S_LOGININFO;
	pk.PlayerInfo = info;
	std::cout << "-> Send:: Login Info Packet [" << pk.PlayerInfo.ID << "] ("
		<< pk.PlayerInfo.X << ","
		<< pk.PlayerInfo.Y << ","
		<< pk.PlayerInfo.Z << ")\n";
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

void Session::send_spawn_monster_packet(int32 id)
{
	Monster* Monster = MonsterMgr.GetMonsterByID(id);
	if (!Monster)
	{
		std::cerr << "Error: Monster with ID " << id << " not found!\n";
		return;
	}

	FSpawnMonsterPacket pk;
	pk.Header.PacketType = EPacketType::S_SPAWN_MONSTER;
	pk.Header.PacketSize = sizeof(FSpawnMonsterPacket);
	pk.MonsterInfo = Monster->GetInfo();

	std::cout << "-> Send:: Spawn Monster[" << pk.MonsterInfo.ID << "]" << std::endl;
	do_send(&pk);
}
