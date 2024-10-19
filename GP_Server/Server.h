#pragma once
#include "Define.h"
#include "Session.h"

class Server {
public:
	void Init();
	void Run();
	
	int32 get_new_id();

private:
	HANDLE h_iocp;
	SOCKET s_socket;
	SOCKET c_socket;

	bool is_running = true;
};
