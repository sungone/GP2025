#include "pch.h"
#include "Server.h"

int main()
{
	LogManager::Init();
	Server& server = Server::GetInst();
	if (server.Init())
		server.Run(); 

	LogManager::Shutdown();
	return 0;
}