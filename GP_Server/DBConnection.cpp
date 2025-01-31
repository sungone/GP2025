#include "pch.h"
#include "DBConnection.h"

bool DBConnection::Connect(const std::wstring& dsn)
{
    SQLRETURN retcode;

    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (retcode != SQL_SUCCESS)
    {
        LOG(LogType::Warning, "Failed to allocate ODBC environment handle.");
        return false;
    }

    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (retcode != SQL_SUCCESS)
    {
        LOG(LogType::Warning, "Failed to set ODBC version attribute.");
        return false;
    }

    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if (retcode != SQL_SUCCESS)
    {
        LOG(LogType::Warning, "Failed to allocate ODBC database handle.");
        return false;
    }

    SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
    retcode = SQLConnectW(hdbc, (SQLWCHAR*)dsn.c_str(), SQL_NTS, nullptr, 0, nullptr, 0);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
        LOG(LogType::Warning, "Query execution failed!");
        return false;
    }

    return true;
}

void DBConnection::Close()
{
    if (hstmt != SQL_NULL_HSTMT) {
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        hstmt = SQL_NULL_HSTMT;
    }
    if (hdbc != SQL_NULL_HDBC) {
        SQLDisconnect(hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        hdbc = SQL_NULL_HDBC;
    }
    if (henv != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        henv = SQL_NULL_HENV;
    }
}

bool DBConnection::ExecuteQuery(const std::wstring& query)
{
    if (hdbc == SQL_NULL_HDBC) return false;

    if (hstmt == SQL_NULL_HSTMT) {
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    }

    SQLRETURN retcode = SQLExecDirectW(hstmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    return retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO;
}