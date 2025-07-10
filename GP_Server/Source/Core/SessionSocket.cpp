#include "pch.h"
#include "SessionSocket.h"
#include "PacketManager.h"

SessionSocket::SessionSocket(SOCKET socket)
	: _socket(socket) {
};

SessionSocket::~SessionSocket()
{
	Close();
}

void SessionSocket::DoRecv()
{
	ZeroMemory(&_recvOver._wsaover, sizeof(_recvOver._wsaover));
	DWORD recv_flag = 0;
	_recvOver._wsabuf.len = BUFSIZE - _remain;
	_recvOver._wsabuf.buf = reinterpret_cast<CHAR*>(_recvOver._buf) + _remain;
	WSARecv(_socket, &_recvOver._wsabuf, 1, 0, &recv_flag, &_recvOver._wsaover, 0);
}

void SessionSocket::DoSend(const Packet* packet)
{
	auto send_data = new ExpOver{ (packet) };
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

void SessionSocket::HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver)
{
	uint32 dataSize = recvByte + _remain;
	if (dataSize >= sizeof(FPacketHeader))
	{
		uint8* buffer = expOver->_buf;
		Packet* packet = reinterpret_cast<Packet*>(buffer);
		while (dataSize >= sizeof(FPacketHeader))
		{
			uint32 packetSize = packet->Header.PacketSize;
			if (packetSize <= 0 || packetSize > dataSize)
				break;

			auto sharedPacket = std::shared_ptr<Packet>(
				reinterpret_cast<Packet*>(new uint8[packet->Header.PacketSize]),
				[](Packet* p) { delete[] reinterpret_cast<uint8_t*>(p); }
			);
			memcpy(sharedPacket.get(), packet, packet->Header.PacketSize);
			SessionManager::GetInst().Schedule(id, [sharedPacket, id]() {
				PacketManager::GetInst().ProcessPacket(id, sharedPacket.get());
				});

			buffer += packetSize;
			dataSize -= packetSize;
			packet = reinterpret_cast<Packet*>(buffer);
		}
		if (dataSize > 0)
			memmove(expOver->_buf, buffer, dataSize);
	}

	_remain = dataSize;
}