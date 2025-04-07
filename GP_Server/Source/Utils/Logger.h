#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <unordered_set>

#define LOG(...) Logger::GetInst().LogMessage(__VA_ARGS__, __FUNCTION__)
#define PRINT(message) Logger::GetInst().PrintNavMesh(message)

#define ENABLE_CONSOLE_LOG       1
#define ENABLE_CONSOLE_WARNING   1
#define ENABLE_CONSOLE_ERROR     1
#define ENABLE_CONSOLE_SEND_LOG  1
#define ENABLE_CONSOLE_RECV_LOG  1

#define ENABLE_FILE_LOG          1
#define ENABLE_FILE_WARNING      1
#define ENABLE_FILE_ERROR        1
#define ENABLE_FILE_SEND_LOG     0
#define ENABLE_FILE_RECV_LOG     0

enum LogType
{
    Error,
    Warning,
    Log,
    RecvLog,
    SendLog,
};

class Logger
{
public:
    static Logger& GetInst()
    {
        static Logger instance;
        return instance;
    }

    void EnableConsoleLog(LogType type) { _enabledConsoleLogs.insert(type); }
    void DisableConsoleLog(LogType type) { _enabledConsoleLogs.erase(type); }

    void EnableFileLog(LogType type) { _enabledFileLogs.insert(type); }
    void DisableFileLog(LogType type) { _enabledFileLogs.erase(type); }

    void PrintNavMesh(const std::string& message)
    {
        std::lock_guard<std::mutex> lock(_logLock);
        PRINT(message);
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
        std::string fullMessage = timeStr + " " + levelStr + "[" + functionName + "] " + message;

        if (_enabledConsoleLogs.count(type))
        {
            std::cout << fullMessage << '\n';
        }

        if (_enabledFileLogs.count(type) && _logFile.is_open())
        {
            _logFile << fullMessage << '\n';
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
    Logger()
    {
#if ENABLE_CONSOLE_LOG
        EnableConsoleLog(Log);
#endif
#if ENABLE_CONSOLE_WARNING
        EnableConsoleLog(Warning);
#endif
#if ENABLE_CONSOLE_ERROR
        EnableConsoleLog(Error);
#endif
#if ENABLE_CONSOLE_SEND_LOG
        EnableConsoleLog(SendLog);
#endif
#if ENABLE_CONSOLE_RECV_LOG
        EnableConsoleLog(RecvLog);
#endif

#if ENABLE_FILE_LOG
        EnableFileLog(Log);
#endif
#if ENABLE_FILE_WARNING
        EnableFileLog(Warning);
#endif
#if ENABLE_FILE_ERROR
        EnableFileLog(Error);
#endif
#if ENABLE_FILE_SEND_LOG
        EnableFileLog(SendLog);
#endif
#if ENABLE_FILE_RECV_LOG
        EnableFileLog(RecvLog);
#endif
    }

    ~Logger()
    {
        CloseLogFile();
    }

    std::string LevelToString(LogType type)
    {
        switch (type)
        {
        case LogType::Error:    return "[Error] ";
        case LogType::Warning:  return "[Warning] ";
        case LogType::Log:      return "[Log] ";
        case LogType::SendLog:  return "[SendLog] >>>> ";
        case LogType::RecvLog:  return "[RecvLog] <<<< ";
        default:                return "[Unknown] ";
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

    std::unordered_set<LogType> _enabledConsoleLogs;
    std::unordered_set<LogType> _enabledFileLogs;
};
