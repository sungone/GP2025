#pragma once

class PlayerSession;
class GameJobScheduler {
public:
    static GameJobScheduler& GetInst()
    {
        static GameJobScheduler inst;
        return inst;
    }
    void Schedule(std::shared_ptr<PlayerSession> session);
    std::shared_ptr<PlayerSession> Pop();

private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::queue<std::shared_ptr<PlayerSession>> _readySessions;
};