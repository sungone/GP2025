#pragma once
class PlayerSession;
using Job = std::function<void()>;

class JobQueue
{
public:
    void Push(Job&& job, PlayerSession* owner);
    void RunNext();

private:
    std::mutex _mutex;
    std::queue<Job> _jobs;
    std::atomic<bool> _running = false;
};
