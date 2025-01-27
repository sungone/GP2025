#include "SessionManager.h"
#include "PacketManager.h"

void SessionManager::Connect(SOCKET& socket)
{
	int id = GenerateId();
	if (id != -1) {
		sessions[id].Connect(socket,id);
		iocp.RegisterSocket(socket, id);
		sessions[id].DoRecv();
	}
}

void SessionManager::Disconnect(int id)
{
	sessions[id].Disconnect();
}

void SessionManager::DoRecv(int id)
{
	sessions[id].DoRecv();
}

void SessionManager::HandleRecvBuffer(int id, int recvByte, ExpOver* expOver)
{
	Session& session = sessions[id];
	int dataSize = recvByte + session.remain;
	char* packet = expOver->buf;
	while (dataSize > 0) {
		int packetSize = packet[1];
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

void SessionManager::Broadcast(Packet* packet, int exptId)
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
	for (int i = 1; i < MAX_CLIENT + 1; ++i)
	{
		if (sessions[i].bLogin) continue;
		return i;
	}
	return -1;
}