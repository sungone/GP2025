#include "pch.h"
#include "DBJobQueue.h"

void DBJobQueue::WorkerLoop()
{
	while (true)
	{
		Run();
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 과도한 CPU 점유 방지
	}
}
