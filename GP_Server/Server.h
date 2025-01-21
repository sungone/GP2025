#pragma once
#include "Define.h"
#include "IOCP.h"
#include "SessionManager.h"
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
				if (key == 'q' || key == 27) // 'q'나 ESC 키
				{
					bRunning = false;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); // CPU 점유율 최소화
		}
	}
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
