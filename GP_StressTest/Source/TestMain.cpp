#include "pch.h"
#include "DummyClinetManager.h"

int main()
{
	auto& TestMgr = DummyClientManager::GetInst();
	if (TestMgr.Init())
		TestMgr.Run();

	return 0;
}