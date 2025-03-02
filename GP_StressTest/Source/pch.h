#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <iostream>
#include <vector>
#include <mutex>
#include <array>

#include "IOCP.h"
#include "FVector.h"
#include "Common.h"
#include "Logger.h"

enum CompType
{
	RECV,
	SEND,
	ACCEPT,

	MOVE,
	ATTACK,
};