#include "pch.h"
#include "DummyClinetManager.h"

bool DummyClientManager::Init()
{
	_hIocp.Init();

	for (int32 i = 0; i < CLIENT_NUM; ++i)
	{
		if (_clients[i].Connect())
		{
			_hIocp.RegisterSocket(_clients[i]._socket, i);

			_clients[i].DoRecv();
			auto accountID = std::format("Test{}", i);
			auto accountPW = std::format("1234");
			auto pkt = LoginPacket(accountID.c_str(), accountPW.c_str());
			_clients[i].DoSend(&pkt);
		}
	}
	LOG("Success Init!");
	return true;
}

void DummyClientManager::Run()
{
	WorkerThread();
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
	}
}

void DummyClientManager::HandleRecv(int32 id, DWORD rbyte, LPWSAOVERLAPPED over)
{
	_clients[id].HandleRecvBuffer(rbyte, reinterpret_cast<ExpOver*>(over));
}