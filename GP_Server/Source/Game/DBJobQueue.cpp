#include "pch.h"
#include "DBJobQueue.h"

void DBJobQueue::WorkerLoop()
{
	while (true)
	{
		Run();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
