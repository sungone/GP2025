#pragma once
#ifndef _DEBUG
#define DB_LOCAL
#endif

inline constexpr int TEST_VALUE = 100;// »½Æ¢±â¿ë

inline constexpr size_t MAX_CLIENT = 10000;
inline constexpr size_t MAX_PLAYER = MAX_CLIENT;
inline constexpr size_t MAX_MONSTER = 500;
inline constexpr size_t MAX_CHARACTER = MAX_PLAYER + MAX_MONSTER;
const std::string MapDataPath = "MapJsonData/";
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