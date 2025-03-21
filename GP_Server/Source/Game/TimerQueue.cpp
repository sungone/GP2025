#include "pch.h"
#include "TimerQueue.h"

std::priority_queue<TimerEvent> TimerQueue::_TimerQueue;
std::mutex TimerQueue::_TimerMutex;

void TimerQueue::TimerThread()
{
	while (true)
	{
		auto now = std::chrono::system_clock::now();

		std::unique_lock<std::mutex> lock(_TimerMutex);
		while (!_TimerQueue.empty() && _TimerQueue.top().wakeUpTime <= now)
		{
			TimerEvent event = _TimerQueue.top();
			_TimerQueue.pop();
			lock.unlock();

			auto over = std::make_shared<ExpOver>();
			switch (event.type)
			{
			case ::RandomMove:
				over->_compType = ::MOVE;
				IOCP::GetInst().PostCompletion(event.id, &over->_wsaover);
				break;
			case ::Chase:
				over->_compType = ::MOVE;
				IOCP::GetInst().PostCompletion(event.id, &over->_wsaover);
				break;
			}

			lock.lock();
		}
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


void TimerQueue::AddTimerEvent(int32 id, EventType type, DWORD delayMs)
{
	std::lock_guard<std::mutex> lock(_TimerMutex);
	auto executionTime = std::chrono::system_clock::now() + std::chrono::milliseconds(delayMs);
	_TimerQueue.push({ id, type, executionTime });
}

