#include "pch.h"
#include "DummyClient.h"

bool DummyClient::Init(uint32 num)
{
	_dummyNum = num;
	_name = std::format("D{:05}", _dummyNum);
	return true;
}

bool DummyClient::Connect(IOCP& hIocp)
{
	if (_connected)
	{
		LOG(Warning, std::format("Already connected - ID{}", _playerId));
		return false;
	}

	_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_socket == INVALID_SOCKET)
	{
		LOG(Error, std::format("WSASocket failed. WSAGetLastError: {}", WSAGetLastError()));
		return false;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET,SERVER_IP, &addr.sin_addr);

	if (connect(_socket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		LOG(Error, std::format("connect failed. WSAGetLastError: {}", errCode));
		closesocket(_socket);
		_socket = INVALID_SOCKET;
		return false;
	}

	hIocp.RegisterSocket(_socket, _dummyNum);
	DoRecv();
	SendSignUpPacket();
	LOG(std::format("Connect success: [{}] sock={}", _name, _socket));
	_connected = true;

	return true;
}

bool DummyClient::Disconnect()
{
	bool status = true;
	if (true == std::atomic_compare_exchange_strong(&_connected, &status, false)) {
		closesocket(_socket);
		return true;
	}
	return false;
}


void DummyClient::DoRecv()
{
	if (_socket == INVALID_SOCKET)
	{
		LOG(Warning, "DoRecv called with invalid socket");
		return;
	}
	ZeroMemory(&_recvOver._wsaover, sizeof(_recvOver._wsaover));
	DWORD recv_flag = 0;
	_recvOver._wsabuf.len = BUFSIZE - _remain;
	_recvOver._wsabuf.buf = reinterpret_cast<CHAR*>(_recvOver._buf) + _remain;
	int res = WSARecv(_socket, &_recvOver._wsabuf, 1, 0, &recv_flag, &_recvOver._wsaover, 0);
	if (res == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		LOG(Error, std::format("WSARecv failed. Error: {}", WSAGetLastError()));
	}
}

void DummyClient::DoSend(Packet* packet)
{
	if (_socket == INVALID_SOCKET)
	{
		LOG(Warning, "DoSend called with invalid socket");
		return;
	}
	auto send_data = new ExpOver{ packet };
	WSASend(_socket, &send_data->_wsabuf, 1, nullptr, 0, &send_data->_wsaover, nullptr);
}

void DummyClient::HandleRecvBuffer(int32 recvByte, ExpOver* expOver)
{
	int32 dataSize = recvByte + _remain;
	uint8* buf = expOver->_buf;
	int32 offset = 0;

	while (dataSize - offset >= sizeof(FPacketHeader))
	{
		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buf + offset);
		int32 packetSize = header->PacketSize;

		if (packetSize < sizeof(FPacketHeader) || packetSize > BUFSIZE)
		{
			break;
		}

		if (dataSize - offset < packetSize)
			break;

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
	case EPacketType::S_LOGIN_FAIL:
		SendSignUpPacket();
		break;
	case EPacketType::S_SIGNUP_FAIL:
		SendLoginPacket();
		break;
	case EPacketType::S_LOGIN_SUCCESS:
	case EPacketType::S_SIGNUP_SUCCESS:
	{
		SendRequestEnterGamePacket();
		break;
	}
	case EPacketType::S_ENTER_GAME:
	{
		auto pkt = reinterpret_cast<EnterGamePacket*>(packet);
		_info = pkt->PlayerInfo;
		_logined = true;
		_active_clients++;
		break;
	}
	case EPacketType::S_PLAYER_MOVE:
	{
		auto pkt = reinterpret_cast<MovePacket*>(packet);
		auto rtt_ms = NowMs() - pkt->MoveTime;
		UpdateDelay(rtt_ms);
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

void DummyClient::SendRequestEnterGamePacket()
{
	auto pkt = RequestEnterGamePacket();
	DoSend(&pkt);
}

bool DummyClient::SendMovePacket()
{
	if (last_move_time + 1s > high_resolution_clock::now()) return false;
	last_move_time = high_resolution_clock::now();

	auto now = NowMs();
	_info.AddState(ECharacterStateType::STATE_WALK);
	MovePacket pkt(_playerId, _info.Pos, _info.State, now);
	DoSend(&pkt);
	return true;
}

bool DummyClient::Move()
{
	static auto& nav = Map::GetInst().GetNavMesh(ZoneType::TUK);


	return true;
}
