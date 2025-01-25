#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

class Timer
{
public:
    Timer() : isRunning(false) {}

    void Start(int intervalMs, std::function<void()> callback)
    {
        isRunning = true;
        timerThread = std::thread([this, intervalMs, callback]() {
            while (isRunning)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
                if (isRunning) 
                    callback();
            }
            });
    }

    void Stop()
    {
        isRunning = false;
        if (timerThread.joinable())
            timerThread.join();
    }

    ~Timer()
    {
        Stop();
    }

private:
    std::atomic<bool> isRunning;
    std::thread timerThread;
};