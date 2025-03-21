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
		while (!_TimerQueue.empty() && _TimerQueue.top()._wakeUpTime <= now)
		{
			TimerEvent event = _TimerQueue.top();
			_TimerQueue.pop();
			lock.unlock();

			auto over = std::make_shared<ExpOver>();
			switch (event._type)
			{
			case ::AI_Patrol:
				over->_compType = ::EVENT_PATROL;
				IOCP::GetInst().PostCompletion(event._id, &over->_wsaover);
				AddTimerEvent(TimerEvent(0, ::AI_Patrol, 2000));
				break;
			}

			lock.lock();
		}
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


void TimerQueue::AddTimerEvent(TimerEvent timerEvent)
{
	std::lock_guard<std::mutex> lock(_TimerMutex);
	_TimerQueue.push(timerEvent);
}

void TimerQueue::AddTimerEvent(TimerType type, uint32 intervalMs)
{
	std::lock_guard<std::mutex> lock(_TimerMutex);
	_TimerQueue.push(TimerEvent(0, type, intervalMs));
}

