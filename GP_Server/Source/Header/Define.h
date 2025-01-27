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

constexpr int MAX_CLIENT = 1000;

constexpr int PKT_TYPE_INDEX = 0;
constexpr int PKT_SIZE_INDEX = 1;

enum CompType { RECV, SEND, ACCEPT };

class ExpOver {
public:
	WSAOVERLAPPED wsaover;
	WSABUF wsabuf;
	char	buf[BUFSIZE];
	CompType compType;
public:
	ExpOver(unsigned char* packet)
	{
		ZeroMemory(&wsaover, sizeof(wsaover));
		wsabuf.len = packet[1];
		wsabuf.buf = buf;
		compType = SEND;
		memcpy(buf, packet, packet[1]);
	}
	ExpOver()
	{
		ZeroMemory(&wsaover, sizeof(wsaover));
		wsabuf.buf = buf;
		wsabuf.len = BUFSIZE;
		compType = RECV;
	}
};