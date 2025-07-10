#pragma once
class JobQueue
{
public:
    void Push(std::function<void()> job);
    void Run();

private:
    std::mutex _mutex;
    std::queue<std::function<void()>> _jobs;
};