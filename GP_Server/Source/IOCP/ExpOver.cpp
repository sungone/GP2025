#include "pch.h"
#include "ExpOver.h"

ExpOver::ExpOver(unsigned char* packet)
{
	ZeroMemory(&_wsaover, sizeof(_wsaover));
	_wsabuf.len = packet[1];
	_wsabuf.buf = _buf;
	_compType = SEND;
	memcpy(_buf, packet, packet[1]);
}

ExpOver::ExpOver()
{
	ZeroMemory(&_wsaover, sizeof(_wsaover));
	_wsabuf.buf = _buf;
	_wsabuf.len = BUFSIZE;
	_compType = RECV;
}