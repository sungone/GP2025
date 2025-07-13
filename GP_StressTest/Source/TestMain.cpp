#include "pch.h"
#include "DummyClinetManager.h"

int main()
{
	LogManager::Init();
	auto& TestMgr = DummyClientManager::GetInst();
	if (TestMgr.Init())
		TestMgr.Run();
	LogManager::Shutdown();
	return 0;
}