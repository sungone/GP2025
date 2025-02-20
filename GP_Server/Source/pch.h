#pragma once
#define NOMINMAX
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
#include <sqlext.h>
#include <chrono>
#include <algorithm>

#include "FVector.h"
#include "Common.h"
#include "Logger.h"
#include "ExpOver.h"
#include "CollisionUtils.h"
#include "RandomUtils.h"

enum CompType
{
	RECV,
	SEND,
	ACCEPT,

	MOVE,
	ATTACK,
};