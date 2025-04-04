#pragma once
#define DB_LOCAL

enum CompType
{
	RECV,
	SEND,
	ACCEPT,
};

enum EventType
{
	MONSTER_UPDATE
};

std::wstring ConvertToWString(const std::string& str)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (size <= 0) return L"";

	std::wstring wstr(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);

	wstr.pop_back();
	return wstr;
}