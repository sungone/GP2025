#pragma once
#include "Define.h"

class Session {

public:

	Session()
	{
		socket = INVALID_SOCKET;
	}

	~Session()
	{
		closesocket(socket);
	}

	void Login()
	{
		static std::default_random_engine dre;
		static std::uniform_real_distribution<float> ud_x(-3000, -1000);
		static std::uniform_real_distribution<float> ud_y(-3500, -1500);

		bLogin = true;
		info.X = ud_x(dre);
		info.Y = ud_y(dre);
		info.Z = 116;
	}

	void DoRecv()
	{
		ZeroMemory(&recvOver.wsaover, sizeof(recvOver.wsaover));
		DWORD recv_flag = 0;
		recvOver.wsabuf.len = BUFSIZE - remain;
		recvOver.wsabuf.buf = recvOver.buf + remain;
		WSARecv(socket, &recvOver.wsabuf, 1, 0, &recv_flag, &recvOver.wsaover, 0);
	}

	void DoSend(void* packet)
	{
		auto send_data = new ExpOver{ reinterpret_cast<unsigned char*>(packet) };
		WSASend(socket, &send_data->wsabuf, 1, nullptr, 0, &send_data->wsaover, nullptr);
	}

	void process_packet(char* packet);

	void send_move_packet(int32 id);
	void send_attack_packet(int32 id);
	void send_login_packet();
	void send_add_player_packet(int32 add_id);
	void send_remove_player_packet(int32 id);

	void Disconnect();
	int32 getId() { return info.ID; }
public:
	bool bLogin = false;
	FPlayerInfo info;
	//int32 id = info.ID;
	SOCKET socket;
	ExpOver recvOver;
	int32 remain;
};
