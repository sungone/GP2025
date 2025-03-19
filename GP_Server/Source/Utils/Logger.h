#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

#define LOG(...) Logger::GetInstance().LogMessage(__VA_ARGS__, __FUNCTION__)
#define PRINT(message) Logger::GetInstance().PrintNavMesh(message)

enum LogType
{
    Warning,
    Log,
    RecvLog,
    SendLog,
};

class Logger
{
public:
    static Logger& GetInstance()
    {
        static Logger instance;
        return instance;
    }

	void PrintNavMesh(const std::string& message)
	{
		std::lock_guard<std::mutex> lock(_logLock);
		std::cout << message << '\n';
	}

    void LogMessage(const std::string& message, const char* functionName)
    {
        LogMessage(LogType::Log, message, functionName);
    }

    void LogMessage(LogType type, const std::string& message, const char* functionName)
    {
        std::lock_guard<std::mutex> lock(_logLock);

        std::string levelStr = LevelToString(type);
        std::string timeStr = GetCurrentTime();

        std::cout << timeStr << " " << levelStr
#ifdef _DEBUG
            << "[" << functionName << "] "
#endif
            << message << '\n';

        if (_logFile.is_open())
        {
            _logFile << timeStr << " " << levelStr << message << '\n';
        }
    }

    void OpenLogFile(const std::string& filename)
    {
        std::lock_guard<std::mutex> lock(_logLock);
        _logFile.open(filename, std::ios::out | std::ios::app);
    }

    void CloseLogFile()
    {
        std::lock_guard<std::mutex> lock(_logLock);
        if (_logFile.is_open())
        {
            _logFile.close();
        }
    }

private:
    Logger() = default;
    ~Logger()
    {
        CloseLogFile();
    }

    std::string LevelToString(LogType type)
    {
        switch (type)
        {
        case LogType::Warning: return "[Warning] ";
        case LogType::Log: return "[Log] ";
        case LogType::SendLog: return "[SendLog] >>>> ";
        case LogType::RecvLog: return "[RecvLog] <<<< ";
        default: return "[Unknown] ";
        }
    }

    std::string GetCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        struct tm timeInfo;
        localtime_s(&timeInfo, &now_c);

        std::ostringstream oss;
        oss << std::put_time(&timeInfo, "[%H:%M:%S]");
        return oss.str();
    }

    std::mutex _logLock;
    std::ofstream _logFile;
};
