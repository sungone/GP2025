#include "pch.h"
#include "Server.h"

int main()
{
    try
    {
        LogManager::Init();
        LOG_I("LogManager initialized.");

        Server& server = Server::GetInst();

        if (!server.Init())
        {
            LOG_E("Server initialization failed.");
            return EXIT_FAILURE;
        }

        LOG_I("Server initialized successfully.");
        server.Run();
        LOG_I("Server stopped.");

        server.Shutdown();
        LOG_I("Server shutdown completed.");
    }
    catch (const std::exception& ex)
    {
        LOG_E(std::format("Unhandled std::exception: {}", ex.what()));
        return EXIT_FAILURE;
    }
    catch (...)
    {
        LOG_E("Unhandled unknown exception.");
        return EXIT_FAILURE;
    }

    LogManager::Shutdown();
    return EXIT_SUCCESS;
}
