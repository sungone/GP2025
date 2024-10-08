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
		ZeroMemory(&recv_over.wsaover, sizeof(recv_over.wsaover));
		DWORD recv_flag = 0;
		recv_over.wsabuf.len = BUFSIZE - prev_remain;
		recv_over.wsabuf.buf = recv_over.send_buf + prev_remain;
		WSARecv(socket, &recv_over.wsabuf, 1, 0, &recv_flag,
			&recv_over.wsaover, 0);
	}

	void do_send(void* packet)
	{
		auto send_data = new EXP_OVER{ reinterpret_cast<unsigned char*>(packet) };
		WSASend(socket, &send_data->wsabuf, 1, nullptr, 0, &send_data->wsaover, nullptr);
	}

	void process_packet(char* packet);

	void send_move_packet(int32 id);
	void send_login_packet();
	void send_add_player_packet(int32 id);
	void send_remove_player_packet(int32 id);
	void disconnect();

public:
	int32	id;
	bool is_login = false;
	FVectorInfo pos;
	SOCKET socket;
	EXP_OVER recv_over;
	int32 prev_remain;
};
