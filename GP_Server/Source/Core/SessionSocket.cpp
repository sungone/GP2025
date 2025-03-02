#include "pch.h"
#include "SessionSocket.h"
#include "PacketManager.h"

SessionSocket::SessionSocket(SOCKET socket)
	: _socket(socket) {};

SessionSocket::~SessionSocket()
{
	Close();
}

void SessionSocket::DoRecv()
{
	ZeroMemory(&_recvOver._wsaover, sizeof(_recvOver._wsaover));
	DWORD recv_flag = 0;
	_recvOver._wsabuf.len = BUFSIZE - _remain;
	_recvOver._wsabuf.buf = _recvOver._buf + _remain;
	WSARecv(_socket, &_recvOver._wsabuf, 1, 0, &recv_flag, &_recvOver._wsaover, 0);
}

void SessionSocket::DoSend(Packet* packet)
{
	auto send_data = new ExpOver{ reinterpret_cast<BYTE*>(packet) };
	WSASend(_socket, &send_data->_wsabuf, 1, nullptr, 0, &send_data->_wsaover, nullptr);
}

void SessionSocket::Init(SOCKET& socket)
{
	this->_socket = socket;
}

void SessionSocket::Close()
{
	closesocket(_socket);
}

void SessionSocket::HandleRecvBuffer(Session& session, int32 recvByte, ExpOver* expOver)
{
	int32 dataSize = recvByte + _remain;
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
	_remain = dataSize;
	if (dataSize > 0)
		memcpy(expOver->_buf, packet, dataSize);
}