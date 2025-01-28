#pragma once
#include "Define.h"
#include "IOCP.h"
#include "SessionManager.h"
#include "GameManager.h"
#include <conio.h>

class Server {
public:
	static Server& GetInst()
	{
		static Server inst;
		return inst;
	}
	~Server() { Close(); }
	void CheckForExitKey()
	{
		while (bRunning)
		{
			if (_kbhit())
			{
				char key = _getch();
				if (key == 'q' || key == 27) // 'q'³ª ESC Å°
				{
					bRunning = false;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	bool Init();
	void Run();
	void Close();

private:
	void CreateWokerThreads();
	void WorkerThreadLoop();
	void HandleError(ExpOver* ex_over, int32 id);
	
	void DoAccept();

	void HandleAccept();
	void HandleRecv(int32 id, int32 recvByte, ExpOver* expOver);

private:
	bool bRunning = true;
	SOCKET listenSocket;
	SOCKET acceptSocket;
	ExpOver acceptOver;

	IOCP& iocp = IOCP::GetInst();
	SessionManager& sessionMgr = SessionManager::GetInst();
	GameManager& gameMgr = GameManager::GetInst();
};
