#include "pch.h"
#include "Server.h"
#include "IOCP.h"
#include "SessionManager.h"
#include "GameWorld.h"
#include "DBJobQueue.h"

bool Server::Init()
{
	SetConsoleOutputCP(CP_UTF8);
#ifdef DB_MODE
	if (!DBManager::GetInst().Connect("localhost", "serverdev", "pass123!", "gp2025"))
	{
		LOG_E("DBManager");
		return false;
	}
#endif
	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		LOG_E("WSAStartup");
		return false;
	}

	InitSocket(_listenSocket, WSA_FLAG_OVERLAPPED);
	if (_listenSocket == INVALID_SOCKET)
	{
		LOG_E("WSASocket");
		return false;
	}

	SOCKADDR_IN addr_s;
	addr_s.sin_family = AF_INET;
	addr_s.sin_port = htons(SERVER_PORT);
	addr_s.sin_addr.s_addr = htonl(ADDR_ANY);

	if (bind(_listenSocket, reinterpret_cast<sockaddr*>(&addr_s), sizeof(addr_s)) == SOCKET_ERROR)
	{
		LOG_E("bind");
		return false;
	}

	if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		LOG_E("listen");
		return false;
	}

	if (!IOCP::GetInst().Init())
	{
		LOG_E("IOCP");
		return false;
	}
	IOCP::GetInst().RegisterSocket(_listenSocket);

	if (!Map::GetInst().Init())
	{
		LOG_E("MapZone");
		return false;
	}

	if (!GameWorld::GetInst().Init())
	{
		LOG_E("GameMgr");
		return false;
	}

	LOG_I("Successfully Init");
	return true;
}

void Server::Run()
{
	LOG_I("Run Server");
	DoAccept();

	static std::vector<std::thread> threads;
	int32 coreNum = std::thread::hardware_concurrency();
	int32 numThreads = coreNum;
	for (int32 i = 0; i < numThreads; ++i) {
		threads.emplace_back([this]() {WorkerThreadLoop(); });
	}
	threads.emplace_back(TimerQueue::TimerThread);
	const int32 jobThreads = std::max(2, coreNum / 4);
	for (int32 i = 0; i < jobThreads; ++i)
		threads.emplace_back([]() { SessionManager::GetInst().GameJobWorkerLoop(); });
#ifdef DB_MODE
	threads.emplace_back([]() { DBJobQueue::GetInst().WorkerLoop(); });
#endif
	for (auto& thread : threads)
	{
		thread.join();
	}
}

void Server::Shutdown()
{
	LOG_I("Shutdown Server");
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
		DWORD errorCode = (ret == FALSE) ? GetLastError() : NO_ERROR;

		if (!ret)
		{
			expOver->errorCode = errorCode;
			HandleCompletionError(expOver, static_cast<int32>(sessionId));
			continue;
		}

		switch (expOver->_compType)
		{
		case CompType::ACCEPT:
			HandleAccept();
			break;
		case CompType::RECV:
			HandleRecv(static_cast<int32>(sessionId), recvByte, expOver);
			break;
		case CompType::SEND:
			delete expOver;
			break;
		}
	}
}

void Server::HandleCompletionError(ExpOver* ex_over, int32 id)
{
	LPVOID msgBuf = nullptr;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		ex_over->errorCode,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPSTR)&msgBuf,
		0,
		nullptr
	);

	std::string errMsg = msgBuf ? (char*)msgBuf : "Unknown error";
	if (msgBuf) LocalFree(msgBuf);

	switch (ex_over->_compType)
	{
	case CompType::ACCEPT:
	{
		LOG_W("CompType : ACCEPT[{}] Code={}", id, ex_over->errorCode);
		break;
	}
	case CompType::RECV:
	{
		LOG_W("CompType : RECV[{}] Code={}", id, ex_over->errorCode);
		GameWorld::GetInst().PlayerLeaveGame(id);
		SessionManager::GetInst().Disconnect(id);
		break;
	}
	case CompType::SEND:
	{
		LOG_W("CompType : SEND[{}] Code={}", id, ex_over->errorCode);
		GameWorld::GetInst().PlayerLeaveGame(id);
		SessionManager::GetInst().Disconnect(id);
		delete ex_over;
		break;
	}
	}

}

void Server::DoAccept()
{
	InitSocket(_acceptSocket, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&_acceptOver._wsaover, sizeof(_acceptOver._wsaover));
	_acceptOver._compType = CompType::ACCEPT;
	AcceptEx(_listenSocket, _acceptSocket, _acceptOver._buf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, 0, &_acceptOver._wsaover);
}

void Server::HandleAccept()
{
	SessionManager::GetInst().Connect(_acceptSocket);
	DoAccept();
}

void Server::HandleRecv(int32 id, int32 recvByte, ExpOver* over)
{
	SessionManager::GetInst().OnRecv(id, recvByte, over);
	SessionManager::GetInst().DoRecv(id);
}

void Server::HandSend(int32 id, ExpOver* over)
{
	SessionManager::GetInst().OnSendCompleted(id, over);
}
