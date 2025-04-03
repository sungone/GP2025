#pragma once

class DBManager
{
public:
    static DBManager& GetInst() {
        static DBManager inst;
        return inst;
    }
    bool Connect(const std::string& host, int port, 
        const std::string& user, const std::string& pass, const std::string& schema);
    void Close();
    void PrintUsers();
    bool InsertUser(const std::string& login_id, const std::string& password, const std::string& nickname);

private:
    std::shared_ptr<mysqlx::Session> _dbsess;
    std::shared_ptr<mysqlx::Schema> _db;

    const std::string USERS_TABLE = "users";

    mysqlx::Table GetUsersTable();
};