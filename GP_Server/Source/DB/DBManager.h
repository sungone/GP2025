#pragma once

class DBManager
{
public:
    static DBManager& GetInst() {
        static DBManager inst;
        return inst;
    }
    bool Connect(const std::string& host, int port, const std::string& user, const std::string& pass, const std::string& schema);
    void Close();

    void PrintUsers();

private:
    std::shared_ptr<mysqlx::Session> _dbsess;
    std::shared_ptr<mysqlx::Schema> _db;
};