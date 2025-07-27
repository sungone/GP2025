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
#include "LogManager.h"
#include "RandomUtils.h"
#include "Map.h"
#include "ExpOver.h"
#include "TimerQueue.h"

using namespace std::chrono;

inline long long NowMs()
{
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

inline int delayTime;
constexpr int DELAY_LIMIT = 100;
constexpr int DELAY_LIMIT2 = 300;
constexpr int ACCEPT_DELY = 40;

inline void UpdateDelay(int rtt_ms) {
	if (delayTime < rtt_ms) delayTime++;
	else if (delayTime > rtt_ms) delayTime--;
}

inline std::atomic_int _active_clients;
inline constexpr size_t MAX_CLIENT = 2000;
inline constexpr size_t MAX_PLAYER = MAX_CLIENT;
inline constexpr float VIEW_DIST = 5000.f;
inline constexpr float playerCollision = 50.f;

const std::string BasePath = "../GP_Server";

const std::string MapDataPath = BasePath + "/MapJsonData/";
const std::string DataTablePath = BasePath + "/DataTable/";

enum class CompType
{
	RECV,
	SEND,
	ACCEPT,
};

static std::wstring ConvertToWString(const std::string& str)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (size <= 0) return L"";

	std::wstring wstr(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);

	wstr.pop_back();
	return wstr;
}