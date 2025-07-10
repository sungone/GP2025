#pragma once

class PlayerSession;
class JobManager {
public:
    static JobManager& GetInst()
    {
        static JobManager inst;
        return inst;
    }
    void Schedule(PlayerSession* session);
    PlayerSession* Pop();

private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::queue<PlayerSession*> _readySessions;
};