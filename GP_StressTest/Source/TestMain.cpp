#include "pch.h"
#include "DummyClinetManager.h"

int main()
{
	LogManager::Init();
	auto& TestMgr = DummyClientManager::GetInst();
	if (!TestMgr.Init())
		return -1;
	try
	{
		TestMgr.Run();
	}
	catch (...)
	{
		TestMgr.Shutdown();
	}
	LogManager::Shutdown();
	return 0;
}