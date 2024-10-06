#pragma once

class Session {

public:

	Session()
	{
		id = 0;
		socket = INVALID_SOCKET;
	}

	~Session()
	{
		closesocket(socket);
	}

	void do_recv()
	{
		ZeroMemory(&recv_over_.wsaover, sizeof(recv_over_.wsaover));
		DWORD recv_flag = 0;
		int res = WSARecv(socket, &recv_over_.wsabuf, 1, nullptr, &recv_flag, &recv_over_.wsaover, nullptr);
		if (0 != res) {
			auto err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				error_display("WSARecv Error : ", WSAGetLastError());
		}
	}

	void do_send(void* packet)
	{
		auto send_data = new EXP_OVER{ reinterpret_cast<unsigned char*>(packet) };
		WSASend(socket, &send_data->wsabuf, 1, nullptr, 0, &send_data->wsaover, nullptr);
	}

	void process_packet(int32 id, uint8_t* packet);

	void send_move_packet(int32 id);
	void send_login_packet();
	void send_add_player_packet(int32 id);

public:
	int32	id;
	bool is_login = false;
	FVectorInfo pos;
	SOCKET socket;
	EXP_OVER recv_over_;
	int32 prev_remain;
};
