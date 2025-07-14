#pragma once
#ifndef _DEBUG
#define DB_MODE
#endif

//for test
inline constexpr int TEST_ATK_WEIGHT = 5;
inline constexpr int TEST_EXP_WEIGHT = 10;

inline constexpr size_t MAX_CLIENT = 3000;
inline constexpr size_t MAX_PLAYER = MAX_CLIENT;
inline constexpr size_t MAX_MONSTER = 500;
inline constexpr size_t MAX_CHARACTER = MAX_PLAYER + MAX_MONSTER;

inline constexpr float VIEW_DIST = 4000.f;
inline constexpr float GRID_CELL_SIZE = 2000.f;
inline constexpr float playerCollision = 50.f;

constexpr int ITEM_DISAPPEAR_TIME_MS = 60 * 1000;
constexpr int MONSTER_RESPAWN_TIME_MS = 10 * 1000;

const std::string BasePath = std::filesystem::current_path().string();

const std::string MapDataPath = BasePath + "/MapJsonData/";
const std::string DataTablePath = BasePath + "/DataTable/";

#define ENUM_NAME(x) (magic_enum::enum_name(x).empty() ? "UnknownEnum" : std::string(magic_enum::enum_name(x)))

enum class CompType
{
	RECV,
	SEND,
	ACCEPT,
};

inline std::wstring ConvertToWString(const std::string& str)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (size <= 0) return L"";

	std::wstring wstr(size - 1, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
	return wstr;
}