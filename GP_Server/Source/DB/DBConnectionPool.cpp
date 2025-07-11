#include "pch.h"
#include "DBConnectionPool.h"

bool DBConnectionPool::Init(const std::string& host, const std::string& user, const std::string& pwd, const std::string& db, int32 poolSize) {
    _host = host; _user = user; _pwd = pwd; _db = db;
    _poolSize = poolSize;

    try {
        for (int i = 0; i < _poolSize; ++i) {
            auto sess = std::make_shared<mysqlx::Session>(mysqlx::SessionSettings(host, user, pwd, db));
            _pool.push(sess);
        }
        return true;
    }
    catch (const mysqlx::Error& e) {
        LOG_E("DB pool init failed: {}", e.what());
        return false;
    }
}

std::shared_ptr<mysqlx::Session> DBConnectionPool::Acquire() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [&]() { return !_pool.empty(); });

    auto session = _pool.front();
    _pool.pop();
    return session;
}

void DBConnectionPool::Release(std::shared_ptr<mysqlx::Session> session) {
    std::lock_guard<std::mutex> lock(_mutex);
    _pool.push(session);
    _cv.notify_one();
}

void DBConnectionPool::Shutdown() {
    std::lock_guard<std::mutex> lock(_mutex);
    while (!_pool.empty()) {
        _pool.front()->close();
        _pool.pop();
    }
}
