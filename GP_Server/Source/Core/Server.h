#pragma once
#include "IOCP.h"
#include "SessionManager.h"
#include "GameWorld.h"
#include "DBConnectionPool.h"

class IOModule
{
public:

};

class Server
{
public:
	static Server& GetInst()
	{
		static Server inst;
		return inst;
	}
	~Server() { Close(); }

	bool Init();
	void Run();
	void Close();

protected:
	void InitSocket(SOCKET& socket, DWORD dwFlags);

	void CreateThreads(std::function<void()> func, int32 numThreads = 1);
	void WorkerThreadLoop();
	void HandleError(ExpOver* ex_over, int32 _id);

	void DoAccept();

	void HandleAccept();
	void HandleRecv(int32 _id, int32 recvByte, ExpOver* expOver);

protected:
	bool _bRunning = true;
	SOCKET _listenSocket;
	SOCKET _acceptSocket;

	ExpOver _acceptOver;
};
