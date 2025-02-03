#include "pch.h"
#include "DBConnectionPool.h"

bool DBConnectionPool::InitPool(const std::wstring& dsn, size_t poolSize)
{
	std::lock_guard<std::mutex> lock(_dMutex);

	for (size_t i = 0; i < poolSize; ++i) {
		std::unique_ptr<DBConnection> conn = std::make_unique<DBConnection>();
		if (!conn->Connect(dsn))
			return false;
		_pool.push(std::move(conn));
	}
	return true;
}

std::unique_ptr<DBConnection> DBConnectionPool::GetConnection()
{
	std::lock_guard<std::mutex> lock(_dMutex);

	if (_pool.empty()) {
		return nullptr;
	}

	std::unique_ptr<DBConnection> conn = std::move(_pool.front());
	_pool.pop();
	return conn;
}

void DBConnectionPool::ReturnConnection(std::unique_ptr<DBConnection> conn)
{
	std::lock_guard<std::mutex> lock(_dMutex);

	_pool.push(std::move(conn));
}