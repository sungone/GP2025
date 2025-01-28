#include "SessionManager.h"
#include "PacketManager.h"

void SessionManager::Connect(SOCKET& socket)
{
	int32 _id = GenerateId();
	if (_id != -1) {
		_sessions[_id].Connect(socket,_id);
		_iocp.RegisterSocket(socket, _id);
		_sessions[_id].DoRecv();
	}
}

void SessionManager::Disconnect(int32 id)
{
	_sessions[id].Disconnect();
}

void SessionManager::DoRecv(int32 id)
{
	_sessions[id].DoRecv();
}

void SessionManager::HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver)
{
	Session& session = _sessions[id];
	int32 dataSize = recvByte + session._remain;
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
	session._remain = dataSize;
	if (dataSize > 0)
		memcpy(expOver->_buf, packet, dataSize);
}

void SessionManager::Broadcast(Packet* packet, int32 exptId)
{
	for (auto& session : _sessions)
	{
		if (!session._bLogin || exptId == session._id)
			continue;
		session.DoSend(packet);
	}
}

int SessionManager::GenerateId()
{
	for (int32 i = 1; i < MAX_CLIENT + 1; ++i)
	{
		if (_sessions[i]._bLogin) continue;
		return i;
	}
	return -1;
}