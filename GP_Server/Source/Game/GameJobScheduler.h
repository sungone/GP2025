#pragma once

class PlayerSession;
class GameJobScheduler {
public:
	static GameJobScheduler& GetInst()
	{
		static GameJobScheduler inst;
		return inst;
	}
public:
	void Schedule(int32 sessionId);
	int32 Pop();

private:
	std::mutex _mutex;
	std::condition_variable _cv;
	std::queue<int32> _readySessionIds;
};