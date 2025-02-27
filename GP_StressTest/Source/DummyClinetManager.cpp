#include "pch.h"
#include "DummyClinetManager.h"

DummyClientManager::DummyClientManager()
{
}

DummyClientManager::~DummyClientManager()
{
	for (int32 i = 0; i < CLIENT_NUM; ++i)
	{
		_clients[i].Disconnect();
	}
	WSACleanup();
}

bool DummyClientManager::Init()
{
	_hIocp.Init();

	for (int32 i = 0; i < CLIENT_NUM; ++i)
	{
		if (_clients[i].Connect())
		{
			_hIocp.RegisterSocket(_clients[i].GetSocket(), i);

			_clients[i].DoRecv();
			_clients[i].DoSend(Packet(C_LOGIN));
		}
	}
	LOG("Success Init!");
	return true;
}

void DummyClientManager::Run()
{
	CreateThreads([this]() { WorkerThread(); }, 1);
	JoinThreads();
}

void DummyClientManager::WorkerThread()
{
	LOG("Run Stress Test");
	DWORD rbyte;
	ULONG_PTR id;
	LPWSAOVERLAPPED over;
	while (true)
	{
		BOOL ret = _hIocp.GetCompletion(rbyte, id, over);
		ExpOver* expOver = reinterpret_cast<ExpOver*>(over);

		if (!ret)
		{
			HandleError(expOver, static_cast<int32>(id));
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
			break;
		}
	}
}

void DummyClientManager::HandleRecv(int32 id, DWORD rbyte, LPWSAOVERLAPPED over)
{
	_clients[id].HandleRecvBuffer(rbyte, reinterpret_cast<ExpOver*>(over));
}

void DummyClientManager::CreateThreads(std::function<void()> func, int32 numThreads)
{
	for (int32 i = 0; i < numThreads; ++i)
		threads.emplace_back(func);
}

void DummyClientManager::JoinThreads()
{
	for (auto& thread : threads)
		thread.join();
}

void DummyClientManager::HandleError(ExpOver* ex_over, int32 id)
{
	switch (ex_over->_compType)
	{
	case RECV:
		LOG(Warning, "CompType : RECV");
		_clients[id].Disconnect();
		break;
	case SEND:
		LOG(Warning, "CompType : SEND");
		delete ex_over;
		break;
	}
}