#include "pch.h"
#include "TimerQueue.h"
#include "GameWorld.h"
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
			case ::MONSTER_UPDATE:
				LOG("Monsters Update Event");
				GameWorld::GetInst().UpdateMonster();
				AddTimerEvent(TimerEvent(0, ::MONSTER_UPDATE, 2000));
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

void TimerQueue::AddTimerEvent(EventType type, uint32 intervalMs)
{
	std::lock_guard<std::mutex> lock(_TimerMutex);
	_TimerQueue.push(TimerEvent(0, type, intervalMs));
}

