#pragma once
#include "IOCP.h"

enum EventType {RandomMove,Chase};
struct TimerEvent
{
	int32 id;
	EventType type;
	std::chrono::system_clock::time_point wakeUpTime;
	bool operator<(const TimerEvent& other) const {
		return wakeUpTime > other.wakeUpTime;
	}
};

class TimerQueue
{
public:
	static void TimerThread();
	static void AddTimerEvent(int32 id, EventType type, DWORD delayMs);
	static std::priority_queue<TimerEvent> _TimerQueue;
	static std::mutex _TimerMutex;
};

