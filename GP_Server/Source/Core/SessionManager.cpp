#include "pch.h"
#include "SessionManager.h"
#include "PacketManager.h"

void SessionManager::Connect(SOCKET& socket)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	int32 _id = GenerateId();
	if (_id != -1) {
		_sessions[_id] = std::make_shared<Session>();
		_sessions[_id]->Connect(socket,_id);
		_iocp.RegisterSocket(socket, _id);
		_sessions[_id]->DoRecv();
	}
}

void SessionManager::Disconnect(int32 id)
{
	_sessions[id]->Disconnect();
}

void SessionManager::DoRecv(int32 id)
{
	_sessions[id]->DoRecv();
}

void SessionManager::HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);

	_sessions[id]->HandleRecvBuffer(recvByte,expOver);

}

void SessionManager::Broadcast(Packet* packet, int32 exptId)
{
	for (auto& session : _sessions)
	{
		if (session == nullptr || exptId == session->GetId())
		//if (!session->IsLogin() || exptId == session->GetId())
			continue;
		session->DoSend(packet);
	}
}

int SessionManager::GenerateId()
{
	for (int32 i = 1; i < MAX_CLIENT; ++i)
	{
		if (_sessions[i] != nullptr) continue;
		//if (_sessions[i]->IsLogin()) continue;
		return i;
	}
	return -1;
}