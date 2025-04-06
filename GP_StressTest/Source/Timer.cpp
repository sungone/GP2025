#include "pch.h"
#include "Timer.h"
#include "ExpOver.h"
#include "DummyClinetManager.h"

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
			case EventType::Move:
				DummyClientManager::GetInst().SendMovePacket(event._id);
				break;
			default:
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
