#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <memory>
#include <vector>

#define LOG_I(...)  spdlog::get("CoreLogger")->info(__VA_ARGS__)
#define LOG_W(...)  spdlog::get("CoreLogger")->warn(__VA_ARGS__)
#define LOG_E(...) spdlog::get("CoreLogger")->error(__VA_ARGS__)
#ifdef _DEBUG
#define LOG_D(...) spdlog::get("CoreLogger")->debug(__VA_ARGS__)
#else
#define LOG_D(...) ((void)0)
#endif

class LogManager
{
public:
    static void Init(spdlog::level::level_enum level = spdlog::level::debug)
    {
        if (_initialized) return;

        constexpr std::size_t queueSize = 8192;
        constexpr std::size_t threadCount = 1;
        spdlog::init_thread_pool(queueSize, threadCount);

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

        std::vector<spdlog::sink_ptr> sinks{ consoleSink };

        _logger = std::make_shared<spdlog::async_logger>(
            "CoreLogger",
            sinks.begin(), sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);

        _logger->set_level(level);
        _logger->flush_on(spdlog::level::err);

        spdlog::register_logger(_logger);

        _initialized = true;
    }

    static void Shutdown()
    {
        if (_initialized)
        {
            spdlog::shutdown();
            _initialized = false;
        }
    }

private:
    static inline std::shared_ptr<spdlog::logger> _logger = nullptr;
    static inline bool _initialized = false;
};
