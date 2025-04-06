#pragma once
#include "DummyClient.h"

constexpr int32 CLIENT_NUM = 10000;
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
	void SendMovePacket(int i);

	void WorkerThread();
	void HandleRecv(int32 id, DWORD rbyte, LPWSAOVERLAPPED over);
	void AdjustClientCount();

private:
	IOCP& _hIocp = IOCP::GetInst();
	std::array<DummyClient, CLIENT_NUM> _clients;
	int32 _connected = 0;
	int32 _nextToConnect = 0;
	int32 _nextToClose = 0;
};

