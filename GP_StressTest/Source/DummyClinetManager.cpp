#include "pch.h"
#include "DummyClinetManager.h"
#include "Timer.h"

bool DummyClientManager::Init()
{
	_hIocp.Init();
	for (uint32 i = 0;i < CLIENT_NUM;++i)
		_clients[i].Init(i);
	return true;
}

void DummyClientManager::Run()
{
	std::thread worker, timer;
	try {
		std::thread worker([this]() { WorkerThread(); });
		std::thread timer([]() { TimerQueue::Get().TimerThread(); });

		worker.join();
		timer.join();
	}
	catch (const std::exception& e) {
		LOG(Error, std::format("Thread error: {}", e.what()));
		throw;
	}

}

void DummyClientManager::SendMovePacket(int i)
{
	_clients[i].SendMovePacket();
	TimerQueue::Get().AddTimerEvent(TimerEvent(i, EventType::Move, 1000));
}

void DummyClientManager::WorkerThread()
{
	LOG("Run Stress Test");
	DWORD rbyte;
	ULONG_PTR id;
	LPWSAOVERLAPPED over;
	static auto lastCheck = std::chrono::steady_clock::now();

	try {
		while (true)
		{
			if (_connected == 0)
			{
				_clients[0].Connect(_hIocp);
				_connected++;
				_nextToConnect++;
			}

			BOOL ret = _hIocp.GetCompletion(rbyte, id, over);
			ExpOver* expOver = reinterpret_cast<ExpOver*>(over);

			if (!ret)
			{
				_clients[id].Disconnect();
				continue;
			}

			switch (expOver->_compType)
			{
			case RECV:
				HandleRecv(static_cast<int32>(id), rbyte, over);
				break;
			case SEND:
				delete over;
				break;
			}
			auto now = steady_clock::now();
			if (now - lastCheck > 100ms)
			{
				AdjustClientCount();
				lastCheck = now;
			}
		}
	}
	catch (const std::exception& e)
	{
		LOG(Error, std::format("Exception in WorkerThread: {}", e.what()));
	}
	catch (...)
	{
		LOG(Error, "Unknown exception in WorkerThread!");
	}
}

void DummyClientManager::HandleRecv(int32 id, DWORD rbyte, LPWSAOVERLAPPED over)
{
	_clients[id].HandleRecvBuffer(rbyte, reinterpret_cast<ExpOver*>(over));
	_clients[id].DoRecv();
}

void DummyClientManager::AdjustClientCount()
{
	using namespace std::chrono;

	static auto lastAdjustTime = steady_clock::now();
	auto now = steady_clock::now();
	auto elapsed = now - lastAdjustTime;

	if (elapsed < 100ms)
		return;

	lastAdjustTime = now;

	int avgDelay = GetAverageDelay();

	if (avgDelay > 150 && _connected > 0) {
		_clients[_nextToClose++].Disconnect();
		_connected--;
		return;
	}

	if (avgDelay < 100 && _connected < CLIENT_NUM) {
		if (_clients[_nextToConnect].Connect(_hIocp)) {
			_connected++;
			_nextToConnect++;
		}
	}
}
