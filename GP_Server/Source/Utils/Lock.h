#pragma once

class Lock
{
public:
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();
private:
	std::atomic<uint32> _lockFlag;
	uint16 _writeCnt;
};

