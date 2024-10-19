#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <iostream>
#include <array>
#include "Proto.h"

enum OPERATION { OP_RECV, OP_SEND, OP_ACCEPT };

class EXP_OVER {
public:
	WSAOVERLAPPED wsaover;
	WSABUF wsabuf;
	char	send_buf[BUFSIZE];
	OPERATION comp_type;
public:
	EXP_OVER(unsigned char* packet)
	{
		ZeroMemory(&wsaover, sizeof(wsaover));
		wsabuf.len = packet[1];
		wsabuf.buf = send_buf;
		comp_type = OP_SEND;
		memcpy(send_buf, packet, packet[1]);
	}
	EXP_OVER()
	{
		ZeroMemory(&wsaover, sizeof(wsaover));
		wsabuf.buf = send_buf;
		wsabuf.len = BUFSIZE;
		comp_type = OP_RECV;
	}
};

void error_display(const char* msg, int err_no);