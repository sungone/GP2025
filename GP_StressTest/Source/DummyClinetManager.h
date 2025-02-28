#pragma once
#include "DummyClient.h"
#include "TimerQueue.h"

constexpr int32 CLIENT_NUM = 100;
class DummyClientManager
{
public:
	static DummyClientManager& GetInst()
	{
		static DummyClientManager inst;
		return inst;
	}
	DummyClientManager();
	~DummyClientManager();

	bool Init();
	void Run();

private:
	void WorkerThread();
	void HandleRecv(int32 id, DWORD rbyte, LPWSAOVERLAPPED over);
	void CreateThreads(std::function<void()> func, int32 numThreads);
	void JoinThreads();
	void HandleError(ExpOver* ex_over, int32 id);
private:
	IOCP& _hIocp = IOCP::GetInst();
	std::array<DummyClient, CLIENT_NUM> _clients;
	std::vector<std::thread> threads;
};

