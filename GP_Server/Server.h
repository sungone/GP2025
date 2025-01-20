#pragma once
#include "Define.h"
#include "IOCP.h"
#include "SessionManager.h"

class Server {
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

private:
	void CreateWokerThreads();
	void WorkerThreadLoop();
	void HandleError(ExpOver* ex_over, int id);
	
	void DoAccept();

	void HandleAccept();
	void HandleRecv(int id, int recvByte, ExpOver* expOver);

private:
	bool bRunning = true;
	SOCKET listenSocket;
	SOCKET acceptSocket;
	ExpOver acceptOver;

	IOCP& iocp = IOCP::GetInst();
	SessionManager& sessionMgr = SessionManager::GetInst();
};
