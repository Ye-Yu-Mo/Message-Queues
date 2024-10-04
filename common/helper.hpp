#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <functional>
#include "logger.hpp"

namespace XuMQ
{
    /*
    封装SqliteHelper类
    提供简单的sqlite数据库操作接口

    1. 创建/打开数据库
    2. 针对打开的数据库执行操作
        1. 表操作
        2. 数据操作
    3. 关闭数据库
    */
    class SqliteHelper
    {
    public:
        typedef int (*SqliteCallback)(void *, int, char **, char **);
        SqliteHelper(const std::string &dbfile)
            : _dbfile(dbfile), _handler(nullptr)
        {
        }
        bool open(int safe_level = SQLITE_OPEN_FULLMUTEX)
        {
            int ret = sqlite3_open_v2(_dbfile.c_str(), &_handler, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | safe_level, nullptr);
            if (ret != SQLITE_OK)
            {
                error(logger, "打开SQLite数据库失败: %s", sqlite3_errmsg(_handler));
                return false;
            }
            return true;
        }
        bool exec(const std::string &sql, SqliteCallback cb, void *arg)
        {
            int ret = sqlite3_exec(_handler, sql.c_str(), cb, arg, nullptr);
            if (ret != SQLITE_OK)
            {
                error(logger, "%s--执行语句失败: %s", sql.c_str(), sqlite3_errmsg(_handler));
                return false;
            }
            return true;
        }
        void close()
        {
            sqlite3_close_v2(_handler);
        }

    private:
        std::string _dbfile;
        sqlite3 *_handler;
    };

}
