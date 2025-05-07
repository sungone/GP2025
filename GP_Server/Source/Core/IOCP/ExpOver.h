#pragma once

enum class CompType;
class ExpOver
{
public:
	ExpOver(const Packet* packet);
	ExpOver();

public:
	WSAOVERLAPPED _wsaover{};
	WSABUF _wsabuf{};
	uint8	_buf[BUFSIZE]{};
	CompType _compType{};
	DWORD errorCode = 0;
};