#pragma once

#include "Define.h"
#include "Session.h"

std::array<Session, MAX_CLIENT> clients;

class Server {
public:
	void Init()
	{
		std::wcout.imbue(std::locale("korean"));
		WSADATA wsa_data;
		auto res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
		s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

		SOCKADDR_IN addr_s;
		addr_s.sin_family = AF_INET;
		addr_s.sin_port = htons(SERVER_PORT);
		addr_s.sin_addr.s_addr = htonl(ADDR_ANY);
		bind(s_socket, reinterpret_cast<sockaddr*>(&addr_s), sizeof(addr_s));
		listen(s_socket, SOMAXCONN);

		h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(s_socket), h_iocp, NULL, 0);

	}

	void Run()
	{
		Init();

		sockaddr c_addr;
		int addr_size = sizeof(c_addr);

		c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		EXP_OVER a_over;
		a_over.comp_type = OP_ACCEPT;
		AcceptEx(s_socket, c_socket, a_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over.wsaover);

		while (is_running) {

			DWORD rw_byte;
			ULONG_PTR key;
			LPWSAOVERLAPPED over;
			BOOL ret = GetQueuedCompletionStatus(h_iocp, &rw_byte, &key, &over, INFINITE);
			EXP_OVER* ex_over = reinterpret_cast<EXP_OVER*>(over);
			if (FALSE == ret) {
				if (ex_over->comp_type == OP_ACCEPT) {
					std::cout << "Accept Error";
					return;
				}
				else {
					std::cout << "GQCS Error on client[" << key << "]\n";
					clients[key].disconnect();
					if (ex_over->comp_type == OP_SEND) delete ex_over;
					continue;
				}
			}
			switch (ex_over->comp_type) {
			case OP_ACCEPT:
			{
				int32 client_id = get_new_id();
				if (client_id != -1) {
					clients[client_id].id = client_id;
					clients[client_id].socket = c_socket;
					CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket),
						h_iocp, client_id, 0);
					clients[client_id].do_recv();
				}
				c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				ZeroMemory(&a_over.wsaover, sizeof(a_over.wsaover));
				AcceptEx(s_socket, c_socket, a_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over.wsaover);
				break;
			}
			case OP_RECV:
			{
				int remain_data = rw_byte + clients[key].prev_remain;
				char* p = ex_over->send_buf;
				while (remain_data > 0) {
					int packet_size = p[1];
					if (packet_size <= remain_data) {
						clients[key].process_packet(p);
						p = p + packet_size;
						remain_data = remain_data - packet_size;
					}
					else break;
				}
				clients[key].prev_remain = remain_data;
				if (remain_data > 0)
					memcpy(ex_over->send_buf, p, remain_data);
				clients[key].do_recv();
				break;
			}
			case OP_SEND:
				delete ex_over;
				break;
			}
		}

		closesocket(s_socket);
		WSACleanup();
	}

	int32 get_new_id()
	{
		static int32 i = 1;
		return i++;
	}

private:

	HANDLE h_iocp;
	SOCKET s_socket;
	SOCKET c_socket;

	bool is_running = true;

	int id;
};

void error_display(const char* msg, int err_no)
{
	WCHAR* msgbuf = nullptr;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&msgbuf, 0, nullptr);
	std::cout << msg;
	std::wcout << msgbuf << std::endl;
	LocalFree(msgbuf);
}
