#include "pch.h"
#include "GameJobScheduler.h"
#include "PlayerSession.h"

void GameJobScheduler::Schedule(std::shared_ptr<PlayerSession> session) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _readySessions.push(session);
    }
    _cv.notify_one();
}

std::shared_ptr<PlayerSession> GameJobScheduler::Pop() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [&]() { return !_readySessions.empty(); });

    auto session = _readySessions.front();
    _readySessions.pop();
    return session;
}
