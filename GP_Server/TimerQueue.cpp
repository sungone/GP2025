#include "pch.h"
#include "TimerQueue.h"

void TimerQueue::TimerThread()
{
    while (true)
    {
        DWORD now = GetTickCount();

        std::unique_lock<std::mutex> lock(_TimerMutex);
        while (!_TimerQueue.empty() && _TimerQueue.top().wakeUpTime <= now)
        {
            TimerEvent event = _TimerQueue.top();
            _TimerQueue.pop();
            lock.unlock();

            std::shared_ptr<std::function<void()>> callback = std::make_shared<std::function<void()>>(event.callback);
            _iocp.PostCompletion(reinterpret_cast<ULONG_PTR>(callback.get()));

            lock.lock();
        }
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


void TimerQueue::AddTimerEvent(CompType type, DWORD delayMs, std::function<void()> callback)
{
    std::lock_guard<std::mutex> lock(_TimerMutex);
    DWORD executionTime = GetTickCount() + delayMs;
    _TimerQueue.push({ type, executionTime, callback });
}

