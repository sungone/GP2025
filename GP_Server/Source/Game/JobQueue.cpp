#include "pch.h"
#include "JobQueue.h"

void JobQueue::Push(std::function<void()> job)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_jobs.push(std::move(job));
}


void JobQueue::Run()
{
	while (true)
	{
		std::function<void()> job;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			if (_jobs.empty())
				break;
			job = std::move(_jobs.front());
			_jobs.pop();
		}
		job();
	}
}