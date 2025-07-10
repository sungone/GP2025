#pragma once
#include "JobQueue.h"

class DBJobQueue : public JobQueue
{
public:
	static DBJobQueue& GetInst()
	{
		static DBJobQueue instance;
		return instance;
	}
	
	void WorkerLoop();
};