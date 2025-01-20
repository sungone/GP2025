#include "Server.h"
#include "SessionManager.h"

bool Server::Init()
{
	std::wcout.imbue(std::locale("korean"));
	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
		return false;
	}
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET) {
		return false;
	}
	SOCKADDR_IN addr_s;
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(SERVER_PORT);
	addr_s.sin_addr.s_addr = htonl(ADDR_ANY);

	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&addr_s), sizeof(addr_s)) == SOCKET_ERROR) {
		return false;
	}

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		return false;
	}

	iocp.Init();
	iocp.RegisterSocket(listenSocket);

	return true;
}

void Server::Run()
{
	DoAccept();
	CreateWokerThreads();
}

void Server::Close()
{
	bRunning = false;

	if (listenSocket != INVALID_SOCKET) {
		closesocket(listenSocket);
		listenSocket = INVALID_SOCKET;
	}

	WSACleanup();
}

void Server::CreateWokerThreads()
{
	int numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	for (int i = 0; i < numThreads; ++i) {
		threads.emplace_back(&Server::WorkerThreadLoop, this);
	}
	for (auto& thread : threads)
		thread.join();
}

void Server::WorkerThreadLoop()
{
	DWORD recvByte;
	ULONG_PTR sessionId;
	LPWSAOVERLAPPED over;

	while (bRunning) {
		BOOL ret = iocp.GetCompletion(recvByte, sessionId, over);
		ExpOver* expOver = reinterpret_cast<ExpOver*>(over);
		if (!ret) {
			HandleError(expOver, sessionId);
			continue;
		}

		switch (expOver->comp_type) {
		case ACCEPT:
		{
			HandleAccept();
			break;
		}
		case RECV:
		{
			HandleRecv(sessionId, recvByte, expOver);
			break;
		}
		case SEND:
			delete expOver;
			break;
		}
	}
}

void Server::HandleError(ExpOver* ex_over, int id)
{
	switch (ex_over->comp_type)
	{
	case ACCEPT:
		LOG(Warning, "CompType : ACCEPT");
		break;
	case RECV:
		LOG(Warning, "CompType : RECV");
		sessionMgr.Disconnect(id);
		break;
	case SEND:
		LOG(Warning, "CompType : SEND");
		sessionMgr.Disconnect(id);
		delete ex_over;
		break;
	}
}

void Server::DoAccept()
{
	ZeroMemory(&acceptOver.wsaover, sizeof(acceptOver.wsaover));
	acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	acceptOver.comp_type = ACCEPT;
	AcceptEx(listenSocket, acceptSocket, acceptOver.buf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, 0, &acceptOver.wsaover);
}

void Server::HandleAccept()
{
	sessionMgr.RegisterSession(acceptSocket);
	DoAccept();
}

void Server::HandleRecv(int id, int recvByte, ExpOver* expOver)
{
	sessionMgr.HandleRecvBuffer(id, recvByte, expOver);
	sessionMgr.DoRecv(id);
}

