#include "pch.h"
#include "DummyClient.h"
#include "Timer.h"
bool DummyClient::Init(uint32 num)
{
	_dummyNum = num;
	_name = std::format("D{:05}", _dummyNum);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		LOG(Error, "WSAStartup");
		return false;
	}

	_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_socket == INVALID_SOCKET)
	{
		LOG(Error, "Socket");
		return false;
	}

	return true;
}

bool DummyClient::Connect(IOCP& hIocp)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if (connect(_socket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		LOG(Error, "connect");
		closesocket(_socket);
		return false;
	}
	hIocp.RegisterSocket(_socket, _dummyNum);
	DoRecv();
	SendSignUpPacket();

	LOG(std::format("Connect - {}", _name));
	return true;
}

void DummyClient::Disconnect()
{
	auto pkt = IDPacket(EPacketType::C_LOGOUT, _info.ID);
	DoSend(&pkt);
	closesocket(_socket);
}

void DummyClient::DoRecv()
{
	ZeroMemory(&_recvOver._wsaover, sizeof(_recvOver._wsaover));
	DWORD recv_flag = 0;
	_recvOver._wsabuf.len = BUFSIZE - _remain;
	_recvOver._wsabuf.buf = reinterpret_cast<CHAR*>(_recvOver._buf) + _remain;
	WSARecv(_socket, &_recvOver._wsabuf, 1, 0, &recv_flag, &_recvOver._wsaover, 0);
}

void DummyClient::DoSend(Packet* packet)
{
	auto send_data = new ExpOver{ packet };
	WSASend(_socket, &send_data->_wsabuf, 1, nullptr, 0, &send_data->_wsaover, nullptr);
}

void DummyClient::HandleRecvBuffer(int32 recvByte, ExpOver* expOver)
{
	int32 dataSize = recvByte + _remain;
	uint8* buf = expOver->_buf;
	int32 offset = 0;
	while (dataSize - offset >= sizeof(FPacketHeader)) {
		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buf + offset);
		int32 packetSize = header->PacketSize;
		if (packetSize <= 0 || packetSize > BUFSIZE)
		{
			LOG(Error, "Invaild Pkt Size");
			break;
		}
		if (dataSize - offset < packetSize) break;

		Packet* packet = reinterpret_cast<Packet*>(buf + offset);
		ProcessPacket(packet);
		offset += packetSize;
	}
	_remain = dataSize - offset;
	if (_remain > 0)
		memmove(buf, buf + offset, _remain);
}

void DummyClient::ProcessPacket(Packet* packet)
{
	auto type = packet->Header.PacketType;
	switch (type)
	{
	case EPacketType::S_LOGIN_SUCCESS:
	{
		auto pkt = reinterpret_cast<LoginSuccessPacket*>(packet);
		_info = pkt->PlayerInfo;
		SendMovePacket();
		TimerQueue::Get().AddTimerEvent(TimerEvent(_dummyNum, EventType::Move, 1000));
		break;
	}
	case EPacketType::S_SIGNUP_SUCCESS:
	{
		auto pkt = reinterpret_cast<SignUpSuccessPacket*>(packet);
		_info = pkt->PlayerInfo;
		SendMovePacket();
		TimerQueue::Get().AddTimerEvent(TimerEvent(_dummyNum, EventType::Move, 1000));
		break;
	}
	case EPacketType::S_LOGIN_FAIL:
		SendSignUpPacket();
		break;
	case EPacketType::S_SIGNUP_FAIL:
		SendLoginPacket();
		break;
	case EPacketType::S_PLAYER_MOVE:
	{
		auto pkt = reinterpret_cast<MovePacket*>(packet);
		auto rtt_ms = NowMs() - pkt->MoveTime;
		UpdateDelaySample(rtt_ms);
		break;
	}
	default:
		break;
	}
}

void DummyClient::SendLoginPacket()
{
	const auto accountID = std::format("{:05}", _dummyNum);
	const auto accountPW = "123";
	auto pkt = LoginPacket(accountID.c_str(), accountPW);
	DoSend(&pkt);
}

void DummyClient::SendSignUpPacket()
{
	const auto accountID = std::format("{:05}", _dummyNum);
	const auto accountPW = "123";
	auto pkt = SignUpPacket(accountID.c_str(), accountPW, _name.c_str());
	DoSend(&pkt);
}

void DummyClient::SendMovePacket()
{
	auto now = NowMs();
	MovePacket pkt(_playerId, _info.Pos, now);
	DoSend(&pkt);
}
