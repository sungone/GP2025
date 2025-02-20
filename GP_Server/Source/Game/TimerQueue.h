#pragma once
#include "IOCP.h"

enum CompType;
struct TimerEvent
{
	CompType type;
	DWORD wakeUpTime;
	std::function<void()> callback;
	bool operator<(const TimerEvent& other) const {
		return wakeUpTime > other.wakeUpTime;
	}
};

class TimerQueue
{
public:
	void TimerThread();
	void AddTimerEvent(CompType type, DWORD delayMs, std::function<void()> callback);
private:
	std::priority_queue<TimerEvent> _TimerQueue;
	std::mutex _TimerMutex;
};

