#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

class Timer
{
public:
	Timer() : _isRunning(false) {}
	~Timer() { Stop(); }
	void Start(int intervalMs, std::function<void()> callback)
	{
		_isRunning = true;
		_timerThread = std::thread([this, intervalMs, callback]() {
			while (_isRunning)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
				if (_isRunning)
					callback();
			}
			});
	}

	void Stop()
	{
		_isRunning = false;
		if (_timerThread.joinable())
			_timerThread.join();
	}

private:
	std::atomic<bool> _isRunning;
	std::thread _timerThread;
};