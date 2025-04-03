#include "pch.h"
#include "Server.h"
#include "IOCP.h"
#include "SessionManager.h"
#include "GameWorld.h"

bool Server::Init()
{
	SetConsoleOutputCP(65001);
	Logger::GetInst().OpenLogFile("gp_server_log.txt");
#ifdef DB_LOCAL
	if (DBManager::GetInst().Connect("localhost", 33060, "serverdev", "pass123!", "gp2025"))
	{
		DBManager::GetInst().PrintUsers();
		LOG(LogType::Error, "DBManager");
		return false;
	}
#endif
	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		LOG(LogType::Error, "WSAStartup");
		return false;
	}

	InitSocket(_listenSocket, WSA_FLAG_OVERLAPPED);
	if (_listenSocket == INVALID_SOCKET)
	{
		LOG(LogType::Error, "WSASocket");
		return false;
	}

	SOCKADDR_IN addr_s;
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(SERVER_PORT);
	addr_s.sin_addr.s_addr = htonl(ADDR_ANY);

	if (bind(_listenSocket, reinterpret_cast<sockaddr*>(&addr_s), sizeof(addr_s)) == SOCKET_ERROR)
	{
		LOG(LogType::Error, "bind");
		return false;
	}

	if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		LOG(LogType::Error, "listen");
		return false;
	}

	if (!IOCP::GetInst().Init())
	{
		LOG(LogType::Error, "IOCP");
		return false;
	}
	IOCP::GetInst().RegisterSocket(_listenSocket);

	if (!MapZone::GetInst().Init())
	{
		LOG(LogType::Error, "MapZone");
		return false;
	}

	if (!GameWorld::GetInst().Init())
	{
		LOG(LogType::Error, "GameMgr");
		return false;
	}

	LOG(LogType::Log, "Successfully initialized");
	return true;
}

void Server::Run()
{
	DoAccept();

	static std::vector<std::thread> threads;
	int32 numThreads = std::thread::hardware_concurrency();
	for (int32 i = 0; i < numThreads; ++i) {
		threads.emplace_back([this]() {WorkerThreadLoop(); });
	}
	threads.emplace_back(TimerQueue::TimerThread);

	for (auto& thread : threads)
	{
		thread.join();
	}
}

void Server::Close()
{
	_bRunning = false;

	if (_listenSocket != INVALID_SOCKET) {
		closesocket(_listenSocket);
		_listenSocket = INVALID_SOCKET;
	}

	WSACleanup();
}

void Server::InitSocket(SOCKET& socket, DWORD dwFlags)
{
	socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, dwFlags);
}

void Server::WorkerThreadLoop()
{
	DWORD recvByte;
	ULONG_PTR sessionId;
	LPWSAOVERLAPPED over;

	while (_bRunning)
	{
		BOOL ret = IOCP::GetInst().GetCompletion(recvByte, sessionId, over);
		ExpOver* expOver = reinterpret_cast<ExpOver*>(over);
		if (!ret)
		{
			HandleCompletionError(expOver, static_cast<int32>(sessionId));
			continue;
		}

		switch (expOver->_compType)
		{
		case ::ACCEPT:
			HandleAccept();
			break;
		case ::RECV:
			HandleRecv(static_cast<int32>(sessionId), recvByte, expOver);
			break;
		case ::SEND:
			delete expOver;
			break;
		}
	}
}

void Server::HandleCompletionError(ExpOver* ex_over, int32 id)
{
	switch (ex_over->_compType)
	{
	case ::ACCEPT:
		LOG(Warning, std::format("CompType : ACCEPT[{}]", id));
		break;
	case ::RECV:
		LOG(Warning, std::format("CompType : RECV[{}]", id));
		SessionManager::GetInst().Disconnect(id);
		break;
	case ::SEND:
		LOG(Warning, std::format("CompType : SEND[{}]", id));
		SessionManager::GetInst().Disconnect(id);
		delete ex_over;
		break;
	}
}

void Server::DoAccept()
{
	InitSocket(_acceptSocket, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&_acceptOver._wsaover, sizeof(_acceptOver._wsaover));
	_acceptOver._compType = ACCEPT;
	AcceptEx(_listenSocket, _acceptSocket, _acceptOver._buf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, 0, &_acceptOver._wsaover);
}

void Server::HandleAccept()
{
	LOG("Accept!");
	SessionManager::GetInst().Connect(_acceptSocket);
	DoAccept();
}

void Server::HandleRecv(int32 _id, int32 recvByte, ExpOver* expOver)
{
	SessionManager::GetInst().HandleRecvBuffer(_id, recvByte, expOver);
	SessionManager::GetInst().DoRecv(_id);
}