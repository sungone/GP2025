#pragma once

enum class EventType
{
	Move
};

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
	static TimerQueue& Get()
	{
		static TimerQueue inst;
		return inst;
	}
	void TimerThread();
	void AddTimerEvent(TimerEvent timerEvent);
	void AddTimerEvent(EventType type, uint32 intervalMs);
private:
	std::priority_queue<TimerEvent> _TimerQueue;
	std::mutex _TimerMutex;
};

