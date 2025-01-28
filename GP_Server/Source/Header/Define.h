#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <memory>
#include <array>
#include <unordered_map>
#include <functional>
#include <random>

#include "Proto.h"
#include "Loger.h"

constexpr uint32 MAX_CLIENT = 1000;

constexpr BYTE PKT_TYPE_INDEX = 0;
constexpr BYTE PKT_SIZE_INDEX = 1;

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