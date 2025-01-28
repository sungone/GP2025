#include "SessionManager.h"
#include "PacketManager.h"

void SessionManager::Connect(SOCKET& socket)
{
	int32 id = GenerateId();
	if (id != -1) {
		sessions[id].Connect(socket,id);
		iocp.RegisterSocket(socket, id);
		sessions[id].DoRecv();
	}
}

void SessionManager::Disconnect(int32 id)
{
	sessions[id].Disconnect();
}

void SessionManager::DoRecv(int32 id)
{
	sessions[id].DoRecv();
}

void SessionManager::HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver)
{
	Session& session = sessions[id];
	int32 dataSize = recvByte + session.remain;
	BYTE* packet = reinterpret_cast<BYTE*>(expOver->buf);
	while (dataSize > 0) {
		int32 packetSize = packet[PKT_SIZE_INDEX];
		if (packetSize <= dataSize) {
			PacketManager::GetInst().ProcessPacket(session, packet);
			packet = packet + packetSize;
			dataSize = dataSize - packetSize;
		}
		else break;
	}
	session.remain = dataSize;
	if (dataSize > 0)
		memcpy(expOver->buf, packet, dataSize);
}

void SessionManager::Broadcast(Packet* packet, int32 exptId)
{
	for (auto& session : sessions)
	{
		if (!session.bLogin || exptId == session.id)
			continue;
		session.DoSend(packet);
	}
}

int SessionManager::GenerateId()
{
	for (int32 i = 1; i < MAX_CLIENT + 1; ++i)
	{
		if (sessions[i].bLogin) continue;
		return i;
	}
	return -1;
}