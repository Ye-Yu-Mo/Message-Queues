/**
 * @file helper.hpp
 * @brief 工具类封装
 * 
 * 此文件定义了 SqliteHelper 和 StrHelper 类，用于简化对 SQLite 数据库的操作。
 * 
 * SqliteHelper 类提供以下功能：
 * - 创建和打开 SQLite 数据库
 * - 执行 SQL 语句，包括表和数据操作
 * - 关闭数据库
 * 
 * StrHelper 类提供字符串处理功能，例如根据指定分隔符分割字符串。
 * 
 * 使用此文件中的类，可以更轻松地与 SQLite 数据库进行交互和管理。
 */

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <functional>
#include "logger.hpp"

namespace XuMQ
{
    /**
     * @class SqliteHelper
     * @brief SQLite 数据库操作助手类
     *
     * 此类封装了 SQLite 数据库的基本操作，包括创建/打开数据库、
     * 执行 SQL 语句和关闭数据库的功能。
     *
     * 主要功能：
     * - 创建和打开数据库
     * - 执行 SQL 语句（包括表操作和数据操作）
     * - 关闭数据库
     */
    class SqliteHelper
    {
    public:
        /**
         * @typedef SqliteCallback
         * @brief SQLite 回调函数类型
         *
         * 回调函数用于处理 SQLite 执行 SQL 语句后的结果。
         *
         * @param arg 用户自定义参数
         * @param count 返回的列数
         * @param values 返回的列值
         * @param names 返回的列名
         * @return 整数值，表示处理结果
         */
        typedef int (*SqliteCallback)(void *, int, char **, char **);
        /**
         * @brief SqliteHelper 构造函数
         * @param dbfile 数据库文件路径
         *
         * 使用给定的数据库文件路径初始化 SqliteHelper 对象。
         */
        SqliteHelper(const std::string &dbfile)
            : _dbfile(dbfile), _handler(nullptr)
        {
        }
        /**
         * @brief 打开数据库
         * @param safe_level 线程安全级别（默认为 SQLITE_OPEN_FULLMUTEX）
         * @return 成功返回 true，失败返回 false
         *
         * 该方法尝试打开指定的 SQLite 数据库，如果失败则记录错误日志。
         */
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
        /**
         * @brief 执行 SQL 语句
         * @param sql 要执行的 SQL 语句
         * @param cb 回调函数，用于处理结果
         * @param arg 用户自定义参数
         * @return 成功返回 true，失败返回 false
         *
         * 该方法执行指定的 SQL 语句，如果失败则记录错误日志。
         */
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
        /**
         * @brief 关闭数据库
         *
         * 该方法关闭打开的 SQLite 数据库。
         */
        void close()
        {
            sqlite3_close_v2(_handler);
        }

    private:
        std::string _dbfile; ///< 数据库文件路径
        sqlite3 *_handler;   ///< SQLite 数据库句柄
    };

    /**
     * @class StrHelper
     * @brief 字符串处理助手类
     *
     * 此类提供字符串处理相关的工具函数，例如字符串分割。
     */
    class StrHelper
    {
        /**
         * @brief 将字符串分割为多个子字符串
         * @param str 要分割的字符串
         * @param sep 分隔符
         * @param result 存储结果的向量
         * @return 返回分割后子字符串的数量
         *
         * 该方法将输入字符串根据指定的分隔符分割并存储在 result 中。
         */
        static size_t split(const std::string &str, const std::string &sep, std::vector<std::string> &result)
        {
            // 从0位置查找指定字符的位置
            // 从上次查找的位置向后继续查找指定位置
            size_t pos, idx = 0;
            while (idx < str.size())
            {
                pos = str.find(sep, idx);
                if (pos == std::string::npos) // 没找到
                {
                    result.push_back(str.substr(idx));
                    return result.size();
                }
                if (pos == idx) // 位置相同则数据无效
                {
                    idx += sep.size();
                    continue;
                }
                result.push_back(str.substr(idx, pos - idx));
                idx = pos + sep.size();
            }
            return result.size();
        }
    };
}
