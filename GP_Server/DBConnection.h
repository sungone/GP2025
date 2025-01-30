#pragma once

class DBConnection {
public:
    DBConnection() : henv(SQL_NULL_HENV), hdbc(SQL_NULL_HDBC), hstmt(SQL_NULL_HSTMT) {}

    ~DBConnection() {
        Close();
    }

    bool Connect(const std::wstring& dsn);
    void Close();
    bool ExecuteQuery(const std::wstring& query);

private:
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt;
};
