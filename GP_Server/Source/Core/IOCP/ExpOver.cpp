#include "pch.h"
#include "ExpOver.h"

ExpOver::ExpOver(const Packet* packet)
{
	ZeroMemory(&_wsaover, sizeof(_wsaover));
	_wsabuf.len = packet->Header.PacketSize;
	_wsabuf.buf = reinterpret_cast<CHAR*>(_buf);
	_compType = CompType::SEND;
	memcpy(_wsabuf.buf, packet, _wsabuf.len);
}

ExpOver::ExpOver()
{
	ZeroMemory(&_wsaover, sizeof(_wsaover));
	_wsabuf.buf = reinterpret_cast<CHAR*>(_buf);
	_wsabuf.len = BUFSIZE;
	_compType = CompType::RECV;
}