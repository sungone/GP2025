#include "SessionManager.h"

void SessionManager::RegisterSession(SOCKET& socket)
{
	int id = GenerateId();
	if (id != -1) {
		clients[id].info.ID = id;
		clients[id].socket = socket;
		iocp.RegisterSocket(socket, id);
		clients[id].DoRecv();
	}
}

void SessionManager::Disconnect(int id)
{
	clients[id].Disconnect();
}

void SessionManager::DoRecv(int id)
{
	clients[id].DoRecv();
}

void SessionManager::HandleRecvBuffer(int id, int recvByte, ExpOver* expOver)
{
	Session& session = clients[id];
	int dataSize = recvByte + session.remain;
	char* packet = expOver->buf;
	while (dataSize > 0) {
		int packetSize = packet[1];
		if (packetSize <= dataSize) {
			session.process_packet(packet);
			packet = packet + packetSize;
			dataSize = dataSize - packetSize;
		}
		else break;
	}
	session.remain = dataSize;
	if (dataSize > 0)
		memcpy(expOver->buf, packet, dataSize);
}
