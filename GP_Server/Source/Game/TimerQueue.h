#pragma once

enum EventType;
struct TimerEvent
{
	int32 _id;
	EventType _type;
	uint32 _intervalMs;
	std::chrono::system_clock::time_point _wakeUpTime;
	TimerEvent(int32 id, EventType type, uint32 intervalMs)
		: _id(id), _type(type), _intervalMs(intervalMs)
	{
		_wakeUpTime = std::chrono::system_clock::now() + std::chrono::milliseconds(intervalMs);
	}
	bool operator<(const TimerEvent& other) const {
		return _wakeUpTime > other._wakeUpTime;
	}
};

class TimerQueue
{
public:
	static void TimerThread();
	static void AddTimerEvent(TimerEvent timerEvent);
	static void AddTimerEvent(EventType type, uint32 intervalMs);
	static std::priority_queue<TimerEvent> _TimerQueue;
	static std::mutex _TimerMutex;
};

