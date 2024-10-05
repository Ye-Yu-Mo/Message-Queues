/**
 * @file helper.hpp
 * @brief 工具类封装
 *
 * 此文件定义了 SqliteHelper StrHelper UUIDHelper FileHelper
 *
 * SqliteHelper 类提供以下功能：
 * - 创建和打开 SQLite 数据库
 * - 执行 SQL 语句，包括表和数据操作
 * - 关闭数据库
 *
 * StrHelper 类提供字符串处理功能，例如根据指定分隔符分割字符串。
 *
 * UUIDHelper 类提供生成静态方法用于生成唯一标识符（UUID），格式为 8-4-4-4-12 的十六进制字符串。
 * 
 * FileHelper 类提供以下功能
 * - 检查文件是否存在
 * - 获取文件大小
 * - 读取和写入文件
 * - 重命名文件
 * - 创建和删除文件及目录
 *
 */

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <functional>
#include <random>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <sys/stat.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
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
    /**
     * @class UUIDHelper
     * @brief 提供生成 UUID 的工具类。
     *
     * 该类包含一个静态方法用于生成唯一标识符（UUID）
     * UUID 格式为 8-4-4-4-12 的十六进制字符串
     */
    class UUIDHelper
    {
    public:
        /**
         * @brief 生成一个随机 UUID。
         *
         * 该方法使用随机设备和伪随机数生成器生成一个 128 位的随机数
         * 将其格式化为 UUID 字符串。UUID 字符串的格式为 8-4-4-4-12，其中每部分用连字符分隔。
         *
         * @return std::string 生成的 UUID 字符串。
         */
        static std::string uuid()
        {
            std::random_device rd;
            std::mt19937_64 generator(rd());                         // 生成机器随机数作为种子 使用梅森旋转算法生成伪随机数
            std::uniform_int_distribution<int> distribution(0, 255); // 范围取到0~255
            std::stringstream ss;
            for (int i = 0; i < 8; i++)
            {
                ss << std::setw(2) << std::setfill('0') << std::hex << distribution(generator); // 转为16进制字符串 不满两位自动补0
                if (i == 3 || i == 5 || i == 7)
                    ss << '-';
            }
            static std::atomic<size_t> seq(1); // 定义原子类型整数
            size_t num = seq.fetch_add(1);
            for (int i = 7; i >= 0; i--)
            {
                ss << std::setw(2) << std::setfill('0') << std::hex << ((num >> i * 8) & 0xff);
                if (i == 6)
                    ss << '-';
            }
            return ss.str();
        }
    };
    /**
     * @class FileHelper
     * @brief 文件操作帮助类
     *
     * 该类提供了一系列静态和实例方法，用于处理文件和目录的操作，包括
     * 检查文件是否存在、获取文件大小、读取和写入文件、重命名文件、创建
     * 和删除文件及目录等功能
     */
    class FileHelper
    {
    public:
        /**
         * @brief 构造函数
         * @param filename 文件名
         *
         * 初始化 FileHelper 对象并设置要操作的文件名。
         */
        FileHelper(const std::string &filename)
            : _filename(filename)
        {
        }
        /**
         * @brief 检查文件是否存在
         * @return true 如果文件存在，false 如果文件不存在
         *
         * 使用 stat 函数检查指定文件是否存在。
         */
        bool exists()
        {
            struct stat st;
            return (stat(_filename.c_str(), &st) == 0);
        }
        /**
         * @brief 获取文件大小
         * @return 文件大小（字节数），如果文件不存在返回 0
         *
         * 使用 stat 函数获取指定文件的大小。
         */
        size_t size()
        {
            struct stat st;
            int ret = stat(_filename.c_str(), &st);
            if (ret < 0)
                return 0;
            return st.st_size;
        }
        /**
         * @brief 读取文件内容
         * @param body 存储读取内容的字符串引用
         * @return true 读取成功，false 读取失败
         *
         * 读取整个文件的内容到给定的字符串中。
         */
        bool read(std::string &body)
        {
            size_t fsize = size();
            body.resize(fsize);
            return read(&body[0], 0, fsize);
        }
        /**
         * @brief 从指定位置读取文件内容
         * @param body 存储读取内容的字符指针
         * @param offset 从文件的哪个位置开始读取
         * @param len 要读取的字节数
         * @return true 读取成功，false 读取失败
         *
         * 从文件中读取指定数量的字节到提供的字符指针中。
         */
        bool read(char *body, size_t offset, size_t len)
        {
            // 打开文件
            std::ifstream ifs(_filename, std::ios::binary | std::ios::in); // 二进制形式打开
            if (ifs.is_open() == false)
            {
                error(logger, "%s:文件打开失败!", _filename.c_str());
                return false;
            }
            // 跳转文件读写位置
            ifs.seekg(offset, std::ios::beg);
            // 读取文件数据
            ifs.read(body, len);
            if (ifs.good() == false)
            {
                error(logger, "%s:文件读取数据失败!", _filename.c_str());
                ifs.close();
                return false;
            }
            // 关闭文件
            ifs.close();
            return true;
        }
        /**
         * @brief 写入字符串到文件
         * @param body 要写入的字符串
         * @return true 写入成功，false 写入失败
         *
         * 将整个字符串的内容写入到文件中。
         */
        bool write(const std::string &body)
        {
            return write(body.c_str(), 0, body.size());
        }
        /**
         * @brief 从指定位置写入数据到文件
         * @param body 要写入的字符指针
         * @param offset 文件中写入的起始位置
         * @param len 要写入的字节数
         * @return true 写入成功，false 写入失败
         *
         * 将指定的字节写入到文件中的给定位置。
         */
        bool write(const char *body, size_t offset, size_t len)
        {
            // 打开文件
            std::fstream fs(_filename, std::ios::binary | std::ios::in | std::ios::out); // 二进制形式打开
            if (fs.is_open() == false)
            {
                error(logger, "%s:文件打开失败!", _filename.c_str());
                return false;
            }
            // 跳转文件指定位置
            fs.seekp(offset, std::ios::beg);
            // 写入数据
            fs.write(body, len);
            if (fs.good() == false)
            {
                error(logger, "%s:文件读取数据失败!", _filename.c_str());
                fs.close();
                return false;
            }
            // 关闭文件
            return true;
        }
        /**
         * @brief 获取文件的父目录
         * @param filename 文件名
         * @return 文件的父目录路径，如果没有父目录则返回 "."
         *
         * 返回指定文件的父目录路径。
         */
        static std::string parentDirectory(const std::string &filename)
        {
            size_t pos = filename.find_last_of("/\\");
            if (pos == std::string::npos)
            {
                return ".";
            }
            std::string path = filename.substr(0, pos);
            return path;
        }
        /**
         * @brief 重命名文件
         * @param nname 新文件名
         * @return true 重命名成功，false 重命名失败
         *
         * 将指定的文件重命名为新的文件名。
         */
        bool rename(const std::string &nname)
        {
            return (::rename(_filename.c_str(), nname.c_str()) == 0);
        }
        /**
         * @brief 创建新文件
         * @param filename 新文件名
         * @return true 创建成功，false 创建失败
         *
         * 在指定路径下创建一个新文件。
         */
        static bool createFile(const std::string filename)
        {
            std::fstream ofs(filename, std::ios::binary | std::ios::out);
            if (ofs.is_open() == false)
            {
                error(logger, "%s:文件创建失败!", filename.c_str());
                return false;
            }
            ofs.close();
            return true;
        }
        /**
         * @brief 删除文件
         * @param filename 要删除的文件名
         * @return true 删除成功，false 删除失败
         *
         * 删除指定的文件。
         */
        static bool removeFile(const std::string filename)
        {
            return (::remove(filename.c_str()) == 0);
        }
        /**
         * @brief 创建目录
         * @param pathname 要创建的目录路径
         * @return true 创建成功，false 创建失败
         *
         * 创建指定路径的目录及其所有父级目录。
         */
        static bool createDirectory(const std::string &pathname)
        {
            size_t pos = 0, idx = 0; // pos表示'/'的位置，idx表示起始位置
            while (idx < pathname.size())
            {
                pos = pathname.find_first_of("/\\", idx); // 找第一个'/'
                if (pos == std::string::npos)             // 如果没有任何目录，直接创建
                {
                    return (mkdir(pathname.c_str(), 0775) == 0);
                }
                std::string parent_dir = pathname.substr(0, pos + 1); // 找到父级目录
                if (FileHelper(parent_dir).exists() == true)          // 如果存在则直接找下一个
                {
                    idx = pos + 1;
                    continue;
                }
                mkdir(parent_dir.c_str(), 0775);
                idx = pos + 1;
            }
            return true;
        }
        /**
         * @brief 删除目录
         * @param pathname 要删除的目录路径
         * @return true 删除成功，false 删除失败
         *
         * 删除指定的目录及其所有内容。
         */
        static bool removeDirectory(const std::string &pathname)
        {

#ifdef _WIN32
            std::string cmd = "rmdir /s /q \"" + pathname + "\"";
#else
            std::string cmd = "rm -rf " + pathname;
#endif
            return (system(cmd.c_str()) != -1);
        }

    private:
        std::string _filename; ///< 操作的文件名
    };
}
