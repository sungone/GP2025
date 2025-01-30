#include "pch.h"
#include "DBConnectionPool.h"

void DBConnectionPool::InitPool(const std::wstring& dsn, size_t poolSize)
{
    RWLock::WriteGuard guard(_dbLock);
    for (size_t i = 0; i < poolSize; ++i) {
        std::unique_ptr<DBConnection> conn = std::make_unique<DBConnection>();
        if (conn->Connect(dsn)) {
            _pool.push(std::move(conn));
        }
    }
}

std::unique_ptr<DBConnection> DBConnectionPool::GetConnection()
{
    RWLock::ReadGuard guard(_dbLock);
    if (_pool.empty()) {
        return nullptr;
    }

    std::unique_ptr<DBConnection> conn = std::move(_pool.front());
    _pool.pop();
    return conn;
}

void DBConnectionPool::ReturnConnection(std::unique_ptr<DBConnection> conn)
{
    RWLock::WriteGuard guard(_dbLock);
    _pool.push(std::move(conn));
}