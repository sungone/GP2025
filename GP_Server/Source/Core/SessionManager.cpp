#include "pch.h"
#include "SessionManager.h"
#include "PacketManager.h"

void SessionManager::Connect(SOCKET& socket)
{
	RWLock::WriteGuard guard(_smgrLock);
	int32 _id = GenerateId();
	if (_id != -1) {
		_sessions[_id].Connect(socket,_id);
		_iocp.RegisterSocket(socket, _id);
		_sessions[_id].DoRecv();
	}
}

void SessionManager::Disconnect(int32 id)
{
	RWLock::ReadGuard guard(_smgrLock);
	_sessions[id].Disconnect();
}

void SessionManager::DoRecv(int32 id)
{
	RWLock::ReadGuard guard(_smgrLock);
	_sessions[id].DoRecv();
}

void SessionManager::HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver)
{
	RWLock::ReadGuard guard(_smgrLock);
	Session& session = _sessions[id];
	int32 dataSize = recvByte + session.GetRemainSize();
	BYTE* packet = reinterpret_cast<BYTE*>(expOver->_buf);
	while (dataSize > 0) {
		int32 packetSize = packet[PKT_SIZE_INDEX];
		if (packetSize <= dataSize) {
			PacketManager::GetInst().ProcessPacket(session, packet);
			packet = packet + packetSize;
			dataSize = dataSize - packetSize;
		}
		else break;
	}
	session.SetRemainSize(dataSize);
	if (dataSize > 0)
		memcpy(expOver->_buf, packet, dataSize);
}

void SessionManager::Broadcast(Packet* packet, int32 exptId)
{
	RWLock::ReadGuard guard(_smgrLock);
	for (auto& session : _sessions)
	{
		if (!session.IsLogin() || exptId == session.GetId())
			continue;
		session.DoSend(packet);
	}
}

int SessionManager::GenerateId()
{
	for (int32 i = 1; i < MAX_CLIENT; ++i)
	{
		if (_sessions[i].IsLogin()) continue;
		return i;
	}
	return -1;
}