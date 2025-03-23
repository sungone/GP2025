#include "pch.h"
#include "DummyClient.h"

DummyClient::DummyClient()
{
}

DummyClient::~DummyClient()
{
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
    auto pkt = IDPacket(EPacketType::C_LOGOUT, _info.ID);
    DoSend(&pkt);
    closesocket(_socket);
}
