#pragma once

enum CompType;
class ExpOver
{
public:
	ExpOver(unsigned char* packet);
	ExpOver();

public:
	WSAOVERLAPPED _wsaover;
	WSABUF _wsabuf;
	char	_buf[BUFSIZE];
	CompType _compType;
};