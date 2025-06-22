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
#include <mysqlx/xdevapi.h>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "ServerDefine.h"
#include "FVector.h"
#include "Common.h"
#include "Logger.h"
#include "ExpOver.h"
#include "RandomUtils.h"
#include "TimerQueue.h"
#include "DBManager.h"

#include "MapZone.h"
#include "ItemTable.h"
#include "PlayerSkillTable.h"
#include "PlayerLevelTable.h"
#include "MonsterTable.h"
#include "QuestTable.h"
#include "SpawnTable.h"