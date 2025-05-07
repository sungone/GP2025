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
#include <unordered_set>
#include <functional>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "IOCP.h"
#include "FVector.h"
#include "Common.h"
#include "Logger.h"
#include "RandomUtils.h"
#include "MapZone.h"
using namespace std::chrono;

enum class CompType
{
	RECV,
	SEND,
	ACCEPT,

	MOVE,
	ATTACK,
};

inline long long NowMs()
{
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

inline int delayTime;
constexpr int DELAY_LIMIT = 100;
constexpr int DELAY_LIMIT2 = 150;
constexpr int ACCEPT_DELY = 50;

inline void UpdateDelay(int rtt_ms) {
	if (delayTime < rtt_ms) delayTime++;
	else if (delayTime > rtt_ms) delayTime--;
}

inline std::atomic_int _active_clients;
const std::string MapDataPath = "../GP_Server/MapJsonData/";
