#pragma once
#include "DBConnectionPool.h"
#include "pch.h"

class ScopedDBSession {
public:
    ScopedDBSession(const ScopedDBSession&) = delete;
    ScopedDBSession& operator=(const ScopedDBSession&) = delete;

    ScopedDBSession() {
        _session = DBConnectionPool::GetInst().Acquire();
    }
    ~ScopedDBSession() {
        DBConnectionPool::GetInst().Release(_session);
    }
    mysqlx::Session& Get() { return *_session; }

private:
    std::shared_ptr<mysqlx::Session> _session;
};
