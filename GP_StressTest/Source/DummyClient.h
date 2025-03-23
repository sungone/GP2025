#pragma once
#include "ExpOver.h"

class DummyClient
{
public:
	DummyClient();
	~DummyClient();

	bool Connect();
	void Disconnect();
	void DoRecv()
	{
		ZeroMemory(&_recvOver._wsaover, sizeof(_recvOver._wsaover));
		DWORD recv_flag = 0;
		_recvOver._wsabuf.len = BUFSIZE - _remain;
		_recvOver._wsabuf.buf = reinterpret_cast<CHAR*>(_recvOver._buf) + _remain;
		WSARecv(_socket, &_recvOver._wsabuf, 1, 0, &recv_flag, &_recvOver._wsaover, 0);
	}

	void DoSend(Packet* packet)
	{
		auto send_data = new ExpOver{ packet };
		std::cout << send_data->_buf;
		WSASend(_socket, &send_data->_wsabuf, 1, nullptr, 0, &send_data->_wsaover, nullptr);
	}

	void HandleRecvBuffer(int32 recvByte, ExpOver* expOver)
	{
		int32 dataSize = recvByte + _remain;
		Packet* packet = reinterpret_cast<Packet*>(expOver->_buf);
		while (dataSize > 0) {
			int32 packetSize = packet->Header.PacketSize;
			if (packetSize <= dataSize) {
				ProcessPacket(packet);
				packet = packet + packetSize;
				dataSize = dataSize - packetSize;
			}
			else break;
		}
		_remain = dataSize;
		if (dataSize > 0)
			memcpy(expOver->_buf, packet, dataSize);
	}

	void ProcessPacket(Packet* packet)
	{
		auto type = packet->Header.PacketType;
		switch (type)
		{
		case EPacketType::S_LOGIN_SUCCESS:
		{
			InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(packet);
			_info = Pkt->Data;
			LOG(RecvLog, std::format("Login Success! [{}]", _id));
			break;
		}
		case EPacketType::S_PLAYER_STATUS_UPDATE:
		{
			InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(packet);
			_info = Pkt->Data;
			break;
		}
		default:
			break;
		}
	}

	SOCKET _socket = INVALID_SOCKET;
private:
	ExpOver _recvOver;
	int32 _remain = 0;
	FInfoData _info;
	int32& _id = _info.ID;
};

