#include "pch.h"
#include "JobManager.h"
#include "PlayerSession.h"

void JobManager::Schedule(PlayerSession* session) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _readySessions.push(session);
    }
    _cv.notify_one();
}

PlayerSession* JobManager::Pop() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [&]() { return !_readySessions.empty(); });

    PlayerSession* session = _readySessions.front();
    _readySessions.pop();
    return session;
}
