#include "Server.h"

int main()
{
	Server& server = Server::GetInst();
	if (server.Init())
		server.Run(); 
}