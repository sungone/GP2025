#include "pch.h"
#include "JobQueue.h"
#include "PlayerSession.h"
#include "JobManager.h"

void JobQueue::Push(Job&& job, PlayerSession* owner)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _jobs.emplace(std::move(job));
    }

    bool expected = false;
    if (_running.compare_exchange_strong(expected, true))
    {
        RunNext();
    }
}

void JobQueue::RunNext()
{
    while (true)
    {
        Job job;

        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_jobs.empty())
            {
                _running.store(false);
                return;
            }

            job = std::move(_jobs.front());
            _jobs.pop();
        }

        job();
    }
}
