#pragma once
#include "DBConnection.h"

class DBConnectionPool {
public:
    static DBConnectionPool& GetInst() {
        static DBConnectionPool inst;
        return inst;
    }

    bool InitPool(const std::wstring& dsn, size_t poolSize);
    std::unique_ptr<DBConnection> GetConnection();
    void ReturnConnection(std::unique_ptr<DBConnection> conn);

private:
    DBConnectionPool() = default;
    ~DBConnectionPool() = default;

    std::queue<std::unique_ptr<DBConnection>> _pool;
    std::mutex _dMutex;
};
