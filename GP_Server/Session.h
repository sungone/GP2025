#pragma once

class Session {

public:

	Session()
	{
		id = 0;
		socket = INVALID_SOCKET;
		recv_over_.comp_type = OP_RECV;
	}

	~Session()
	{
		closesocket(socket);
	}

	void do_recv()
	{
		ZeroMemory(&recv_over_.wsaover, sizeof(recv_over_.wsaover));
		DWORD recv_flag = 0;
		int res = WSARecv(socket, recv_over_.wsabuf, 1, nullptr, &recv_flag, &recv_over_.wsaover, nullptr);
		if (0 != res) {
			auto err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				error_display("WSARecv Error : ", WSAGetLastError());
		}
	}

	void do_send(char s_id, char* buf, int m_size)
	{
		auto b = new EXP_OVER(s_id, buf, m_size);
		b->comp_type = OP_SEND;
		WSASend(socket, b->wsabuf, 1, nullptr, 0, &b->wsaover, nullptr);
	}

	void process_packet(DWORD transfer_size);

public:
	int32	id;
	SOCKET socket;
	EXP_OVER recv_over_;
};
