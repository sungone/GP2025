#include "pch.h"
#include "SessionManager.h"
#include "PacketManager.h"

void SessionManager::Connect(SOCKET& socket)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	int32 _id = GenerateId();
	if (_id != -1) {
		_sessions[_id] = std::make_shared<Session>();
		_sessions[_id]->Connect(socket, _id);
		_iocp.RegisterSocket(socket, _id);
		_sessions[_id]->DoRecv();
	}
}

void SessionManager::Disconnect(int32 id)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	_sessions[id]->Disconnect();
	_sessions[id] = nullptr;
}

void SessionManager::DoRecv(int32 id)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	if (_sessions[id] == nullptr) return;
	_sessions[id]->DoRecv();
}

void SessionManager::HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver)
{
	_sessions[id]->HandleRecvBuffer(recvByte, expOver);
}

void SessionManager::HandleLogin(int32 id)
{
	_sessions[id]->Login();
	auto& playerInfo = _sessions[id]->GetPlayerInfo();
	auto loginPkt = InfoPacket(EPacketType::S_LOGIN_SUCCESS, playerInfo);
	_sessions[id]->DoSend(&loginPkt);
	auto myInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, playerInfo);
	Broadcast(&myInfoPkt, id);

	for (auto& cl : _sessions)
	{
		if (cl == nullptr) continue;
		if (cl->GetId() == id)
			continue;
		auto otherInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, cl->GetPlayerInfo());
		_sessions[id]->DoSend(&otherInfoPkt);
	}
}

void SessionManager::SendPacket(int32 sessionId, Packet* packet)
{
	_sessions[sessionId]->DoSend(packet);
}

void SessionManager::Broadcast(Packet* packet, int32 exptId)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	for (auto& session : _sessions)
	{
		if (session == nullptr || exptId == session->GetId())
			continue;
		session->DoSend(packet);
	}
}

int SessionManager::GenerateId()
{
	for (int32 i = 1; i < MAX_CLIENT; ++i)
	{
		if (_sessions[i] != nullptr) continue;
		return i;
	}
	return -1;
}