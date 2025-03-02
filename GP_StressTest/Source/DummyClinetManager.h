#pragma once
#include "DummyClient.h"

constexpr int32 CLIENT_NUM = 50;
class DummyClientManager
{
public:
	static DummyClientManager& GetInst()
	{
		static DummyClientManager inst;
		return inst;
	}
	bool Init();
	void Run();

	void WorkerThread();
	void HandleRecv(int32 id, DWORD rbyte, LPWSAOVERLAPPED over);
private:
	IOCP& _hIocp = IOCP::GetInst();
	std::array<DummyClient, CLIENT_NUM> _clients;
};

