#include "pch.h"
#include "DummyClient.h"

DummyClient::DummyClient()
{
}

DummyClient::~DummyClient()
{
    closesocket(_socket);
}

bool DummyClient::Connect()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed.\n";
        return false;
    }

    _socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (_socket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed.\n";
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if (connect(_socket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to server.\n";
        closesocket(_socket);
        return false;
    }

    return true;
}

void DummyClient::Disconnect()
{
    DoSend(Packet(C_LOGIN));
}

void DummyClient::DoRecv()
{
    ZeroMemory(&_recvOver._wsaover, sizeof(_recvOver._wsaover));
    DWORD recv_flag = 0;
    _recvOver._wsabuf.len = BUFSIZE - _remain;
    _recvOver._wsabuf.buf = _recvOver._buf + _remain;
    WSARecv(_socket, &_recvOver._wsabuf, 1, 0, &recv_flag, &_recvOver._wsaover, 0);
}

void DummyClient::DoSend(Packet&& packet)
{
    auto send_data = new ExpOver{ reinterpret_cast<BYTE*>(new Packet(std::move(packet))) };
    WSASend(_socket, &send_data->_wsabuf, 1, nullptr, 0, &send_data->_wsaover, nullptr);
}

void DummyClient::HandleRecvBuffer(int32 recvByte, ExpOver* expOver)
{
    int32 dataSize = recvByte + _remain;
    BYTE* packet = reinterpret_cast<BYTE*>(expOver->_buf);
    while (dataSize > 0) {
        int32 packetSize = packet[PKT_SIZE_INDEX];
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

void DummyClient::ProcessPacket(BYTE* packet)
{
    FPacketHeader* ph = reinterpret_cast<FPacketHeader*>(packet);
    switch (ph->PacketType)
    {
    case EPacketType::S_LOGIN_SUCCESS:
    {
        LOG(RecvLog, "Login Sucess");
        InfoPacket* Pkt = reinterpret_cast<InfoPacket*>(packet);
        _info = Pkt->Data;
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

void DummyClient::Move()
{
}
