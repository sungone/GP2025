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
	WSABUF wsabuf[1];
	char	send_buf[BUFSIZE];
	OPERATION comp_type;
public:
	EXP_OVER(char s_id, char* mess, int m_size)
	{
		ZeroMemory(&wsaover, sizeof(wsaover));
		wsabuf[0].buf = send_buf;
		wsabuf[0].len = m_size + 2;
		send_buf[0] = m_size + 2;
		send_buf[1] = s_id;
		memcpy(send_buf + 2, mess, m_size);
	}
	EXP_OVER()
	{
		ZeroMemory(&wsaover, sizeof(wsaover));
		wsabuf[0].buf = send_buf;
		wsabuf[0].len = BUFSIZE;
	}
};

void error_display(const char* msg, int err_no);
