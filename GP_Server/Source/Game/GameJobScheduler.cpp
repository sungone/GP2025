#include "pch.h"
#include "GameJobScheduler.h"
#include "PlayerSession.h"

void GameJobScheduler::Schedule(int32 sessionId) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _readySessionIds.push(sessionId);
    }
    _cv.notify_one();
}

int32 GameJobScheduler::Pop() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [&]() { return !_readySessionIds.empty(); });

    int32 sessionId = _readySessionIds.front();
    _readySessionIds.pop();
    return sessionId;
}