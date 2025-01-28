#include "Server.h"
#include "SessionManager.h"
#include "GameManager.h"

bool Server::Init()
{
	std::wcout.imbue(std::locale("korean"));
	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
		return false;
	}
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (_listenSocket == INVALID_SOCKET) {
		return false;
	}
	SOCKADDR_IN addr_s;
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(SERVER_PORT);
	addr_s.sin_addr.s_addr = htonl(ADDR_ANY);

	if (bind(_listenSocket, reinterpret_cast<sockaddr*>(&addr_s), sizeof(addr_s)) == SOCKET_ERROR) {
		return false;
	}

	if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		return false;
	}

	_iocp.Init();
	_iocp.RegisterSocket(_listenSocket);

	_gameMgr.Init();

	return true;
}

void Server::Run()
{
	/*std::thread inputThread(&Server::CheckForExitKey,this);
	inputThread.join();*/

	DoAccept();
	//CreateWokerThreads();
	WorkerThreadLoop();
}

void Server::Close()
{
	_bRunning = false;

	_gameMgr.StopMonsterStateBroadcast();
	_gameMgr.StopMonsterAIUpdate();

	if (_listenSocket != INVALID_SOCKET) {
		closesocket(_listenSocket);
		_listenSocket = INVALID_SOCKET;
	}

	WSACleanup();
}

void Server::CreateWokerThreads()
{
	int32 numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	for (int32 i = 0; i < numThreads; ++i) {
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

	while (_bRunning) {
		BOOL ret = _iocp.GetCompletion(recvByte, sessionId, over);
		ExpOver* expOver = reinterpret_cast<ExpOver*>(over);
		if (!ret) {
			HandleError(expOver, sessionId);
			continue;
		}

		switch (expOver->_compType) {
		case ACCEPT:
			HandleAccept();
			break;
		case RECV:
			HandleRecv(sessionId, recvByte, expOver);
			break;
		case SEND:
			delete expOver;
			break;
		}
	}
}

void Server::HandleError(ExpOver* ex_over, int32 id)
{
	switch (ex_over->_compType)
	{
	case ACCEPT:
		LOG(Warning, "CompType : ACCEPT");
		break;
	case RECV:
		LOG(Warning, "CompType : RECV");
		_sessionMgr.Disconnect(id);
		break;
	case SEND:
		LOG(Warning, "CompType : SEND");
		_sessionMgr.Disconnect(id);
		delete ex_over;
		break;
	}
}

void Server::DoAccept()
{
	ZeroMemory(&_acceptOver._wsaover, sizeof(_acceptOver._wsaover));
	_acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	_acceptOver._compType = ACCEPT;
	AcceptEx(_listenSocket, _acceptSocket, _acceptOver._buf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, 0, &_acceptOver._wsaover);
}

void Server::HandleAccept()
{
	_sessionMgr.Connect(_acceptSocket);
	DoAccept();
}

void Server::HandleRecv(int32 _id, int32 recvByte, ExpOver* expOver)
{
	_sessionMgr.HandleRecvBuffer(_id, recvByte, expOver);
	_sessionMgr.DoRecv(_id);
}

