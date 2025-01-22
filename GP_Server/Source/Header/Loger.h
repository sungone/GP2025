#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>

#define LOG(type, message) \
    Logger::GetInstance().LogMessage(type, message, __FUNCTION__)

enum LogType
{
	Warning,
	Log,
	RecvLog,
	SendLog,
};

class Logger {
public:
	static Logger& GetInstance()
	{
		static Logger instance;
		return instance;
	}

	void LogMessage(LogType type, const std::string& message, const char* functionName)
	{
		std::lock_guard<std::mutex> lock(logMutex);

		std::string levelStr = LevelToString(type);

		std::cout
			<< levelStr
#ifdef _DEBUG
			<< "[" << functionName << "] "
#endif
			<< message << '\n';
	}

private:
	Logger() = default;

	std::string LevelToString(LogType type)
	{
		switch (type)
		{
		case LogType::Warning: return "[Warning] ";
		case LogType::Log: return "[Log] ";
		case LogType::SendLog: return "[<<SendLog] ";
		case LogType::RecvLog: return "[>>RecvLog]  ";
		default: return "Unknown";
		}
	}

	std::mutex logMutex;
};
