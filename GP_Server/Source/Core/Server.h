#pragma once
#include "IOCP.h"
#include "SessionManager.h"
#include "GameManager.h"
#include "DBConnectionPool.h"

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
	void HandleError(ExpOver* ex_over, int32 _id);
	
	void DoAccept();

	void HandleAccept();
	void HandleRecv(int32 _id, int32 recvByte, ExpOver* expOver);

private:
	bool _bRunning = true;
	SOCKET _listenSocket;
	SOCKET _acceptSocket;
	ExpOver _acceptOver;

	IOCP& _iocp = IOCP::GetInst();
	SessionManager& _sessionMgr = SessionManager::GetInst();
	GameManager& _gameMgr = GameManager::GetInst();
	DBConnectionPool& _dbPool = DBConnectionPool::GetInst();
};
