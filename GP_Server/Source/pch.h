#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <iostream>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <memory>
#include <array>
#include <queue>
#include <unordered_map>
#include <functional>
#include <random>
#include <sqlext.h>
#include <chrono>

#include "Proto.h"
#include "Loger.h"
#include "ExpOver.h"

enum CompType
{
	RECV,
	SEND,
	ACCEPT,

	MOVE,
	ATTACK,
};