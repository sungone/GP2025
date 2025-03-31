#include "pch.h"
#include "Server.h"

int main()
{
	std::wcout.imbue(std::locale("korean"));
	Logger::GetInst().OpenLogFile("gp_server_log.txt");

	Server& server = Server::GetInst();
	if (server.Init())
		server.Run(); 
}