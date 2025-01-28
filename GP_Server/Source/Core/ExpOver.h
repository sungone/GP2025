#pragma once

enum CompType { RECV, SEND, ACCEPT };

class ExpOver
{
public:
	ExpOver(unsigned char* packet)
	{
		ZeroMemory(&_wsaover, sizeof(_wsaover));
		_wsabuf.len = packet[1];
		_wsabuf.buf = _buf;
		_compType = SEND;
		memcpy(_buf, packet, packet[1]);
	}
	ExpOver()
	{
		ZeroMemory(&_wsaover, sizeof(_wsaover));
		_wsabuf.buf = _buf;
		_wsabuf.len = BUFSIZE;
		_compType = RECV;
	}
public:
	WSAOVERLAPPED _wsaover;
	WSABUF _wsabuf;
	char	_buf[BUFSIZE];
	CompType _compType;
};