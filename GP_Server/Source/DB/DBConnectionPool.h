#pragma once

class DBConnectionPool {
public:
    static DBConnectionPool& GetInst() {
        static DBConnectionPool inst;
        return inst;
    }

    bool Init(const std::string& host, const std::string& user, const std::string& pwd, const std::string& db, int32 poolSize);
    void Shutdown();

    std::shared_ptr<mysqlx::Session> Acquire();
    void Release(std::shared_ptr<mysqlx::Session> session);

private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::queue<std::shared_ptr<mysqlx::Session>> _pool;
    std::string _host, _user, _pwd, _db;
    int32 _poolSize = 0;
};
