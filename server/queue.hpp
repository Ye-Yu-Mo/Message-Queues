/**
 * @file queue.hpp
 * @brief 消息队列模块的定义和实现
 *
 * 本文件定义了消息队列的结构体 `MsgQueue` 及其管理类 `MsgQueueMapper` 和 `MsgQueueManager`。
 * 
 * `MsgQueue` 结构体用于表示一个消息队列，包含其属性和相关操作。
 * `MsgQueueMapper` 类负责持久化管理，将消息队列信息存储在 SQLite 数据库中。
 * `MsgQueueManager` 类负责在内存中管理消息队列，并提供相应的操作接口。
 */

#pragma once
#include "../common/logger.hpp"
#include "../common/helper.hpp"
#include "../common/msg.pb.h"
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <memory>

namespace XuMQ
{
    /// @struct MsgQueue
    /// @brief 消息队列结构体
    struct MsgQueue
    {
        using ptr = std::shared_ptr<MsgQueue>;             ///< 消息队列指针
        std::string name;                                  ///< 消息队列名称
        bool durable;                                      ///< 持久化标志
        bool exclusive;                                    ///< 独占标志
        bool auto_delete;                                  ///< 自动删除标志
        std::unordered_map<std::string, std::string> args; ///< 其他参数
        /// @brief 无参构造
        MsgQueue() {}
        /// @brief 构造函数
        /// @param qname 消息队列名称
        /// @param qdurable 持久化标志
        /// @param qexclusive 独占标志
        /// @param qauto_delete 自动删除标志
        /// @param qargs 其他参数
        MsgQueue(const std::string &qname,
                 bool qdurable,
                 bool qexclusive,
                 bool qauto_delete,
                 std::unordered_map<std::string, std::string> &qargs) : name(qname), durable(qdurable), exclusive(qexclusive),
                                                                        auto_delete(qauto_delete), args(qargs)
        {
        }
        /// @brief 解析字符串并存储到映射成员中
        /// @param str_args 从数据库获取的字符串
        /// @note
        /// args存储键值对 在数据库中会使用格式字符串进行存储
        /// 格式为'key=val&key=val...'
        void setArgs(const std::string &str_args)
        {
            std::vector<std::string> sub_args;
            StrHelper::split(str_args, "&", sub_args);
            for (auto &str : sub_args)
            {
                size_t pos = str.find('=');
                std::string key = str.substr(0, pos);
                std::string value = str.substr(pos + 1);
                args.insert(std::make_pair(key, value));
            }
        }
        /// @brief 将映射成员转化为字符串
        /// @return 转化成的字符串
        /// @note
        /// 字符串格式为'key=value&key=value...'
        std::string getArgs()
        {
            std::string result;
            for (auto &arg : args)
            {
                result += (arg.first + "=" + arg.second + "&");
            }
            if (!result.empty())
                result.pop_back(); // 去除最后一个'&'
            return result;
        }
    };
    using QueueMap = std::unordered_map<std::string, MsgQueue::ptr>; ///< 消息队列映射表 消息队列名称->消息队列指针
    /// @class MsgQueueMapper
    /// @brief 消息队列持久化管理类 将数据存储在sqlite数据库中
    class MsgQueueMapper
    {
    public:
        /// @brief 消息队列持久化管理类 构造函数
        /// @param dbfile 数据库名称
        /// @note 如果数据库不存在则自动创建
        MsgQueueMapper(const std::string &dbfile)
            : _sql_helper(dbfile)
        {
            std::string path = FileHelper::parentDirectory(dbfile);
            FileHelper::createDirectory(path);
            assert(_sql_helper.open());
            createTable();
        }
        /// @brief 创建一张表
        void createTable()
        {
            const char *CREATE_TABLE = "create table if not exists queue_table(name varchar(32) primary key,\
            durable tinyint, exclusive tinyint,\
            auto_delete tinyint, args varchar(128));";
            bool ret = _sql_helper.exec(CREATE_TABLE, nullptr, nullptr);
            if (ret == false)
            {
                fatal(logger, "创建消息队列数据库表失败!");
                abort();
            }
        }
        /// @brief 移除一张表
        void removeTable()
        {
            const char *DROP_TABLE = "drop table if exists queue_table;";
            bool ret = _sql_helper.exec(DROP_TABLE, nullptr, nullptr);
            if (ret == false)
            {
                fatal(logger, "删除消息队列数据库表失败!");
                abort();
            }
        }
        /// @brief 新增一个消息队列
        /// @param queue 消息队列指针
        /// @return 新增成功返回true 失败返回false并输出日志
        bool insert(MsgQueue::ptr &queue)
        {
            const char *INSERT_SQL = "insert into queue_table values('%s', %d, %d, %d, '%s');";
            char sql_str[4096] = {0};
            sprintf(sql_str, INSERT_SQL, queue->name.c_str(), queue->durable, queue->exclusive,
                    queue->auto_delete, queue->getArgs().c_str());
            bool ret = _sql_helper.exec(sql_str, nullptr, nullptr);
            if (ret == false)
            {
                error(logger, "数据库: 插入消息队列失败!");
                return false;
            }
            return true;
        }
        /// @brief 移除一个消息队列
        /// @param name 消息队列名称
        /// @return 删除成功返回true 删除失败输出日志并返回false
        bool remove(const std::string &name)
        {
            const char *DELETE_SQL = "delete from queue_table where name = '%s';";
            char sql_str[4096] = {0};
            sprintf(sql_str, DELETE_SQL, name.c_str());
            bool ret = _sql_helper.exec(sql_str, nullptr, nullptr);
            if (ret == false)
            {
                error(logger, "数据库: 删除消息队列失败!");
                return false;
            }
            return true;
        }
        /// @brief 获取所有消息队列 从数据库加载到内存
        /// @return 一张映射表 消息队列名称->消息队列指针
        /// @see MsgQueue::ptr MsgQueueManager
        QueueMap recovery()
        {
            QueueMap result;
            const char *SELECT_SQL = "select * from queue_table;";
            _sql_helper.exec(SELECT_SQL, selectCallback, &result);
            return result;
        }

    private:
        /// @brief select语句的回调函数 将获取到的数据存入参数中
        /// @param arg 存入的参数
        /// @param numcol 列数
        /// @param row 行
        /// @param fields 列名
        /// @return 处理结果 0表示正常退出
        static int selectCallback(void *arg, int numcol, char **row, char **fields)
        {
            QueueMap *result = (QueueMap *)arg;
            auto mqp = std::make_shared<MsgQueue>();
            mqp->name = row[0];
            mqp->durable = (bool)std::stoi(row[1]);
            mqp->exclusive = (bool)std::stoi(row[2]);
            mqp->auto_delete = (bool)std::stoi(row[3]);
            if (row[4])
                mqp->setArgs(row[4]);
            result->insert(std::make_pair(mqp->name, mqp));
            return 0;
        }

    private:
        SqliteHelper _sql_helper; ///< 数据库操作对象
    };
    /// @class MsgQueueManager
    /// @brief 消息队列数据内存管理类
    class MsgQueueManager
    {
    public:
        using ptr = std::shared_ptr<MsgQueueManager>; ///< 消息队列数据内存管理指针
        /// @brief 消息队列数据内存管理类 构造函数 从数据库中恢复数据
        /// @param dbfile 数据库名称
        MsgQueueManager(const std::string &dbfile)
            : _mapper(dbfile)
        {
            _queues = _mapper.recovery();
        }
        /// @brief 声明消息队列
        /// @param qname 消息队列名称
        /// @param qdurable 数据持久化标志
        /// @param qexclusive 独占标志
        /// @param qauto_delete 自动删除标志
        /// @param qargs 其他参数
        /// @return 声明成功返回true 失败返回false
        bool declareQueue(const std::string &qname,
                          bool qdurable,
                          bool qexclusive,
                          bool qauto_delete,
                          std::unordered_map<std::string, std::string> &qargs)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _queues.find(qname);
            if (it != _queues.end())
                return true;
            auto mqp = std::make_shared<MsgQueue>(qname, qdurable, qexclusive, qauto_delete, qargs);
            if (qdurable)
            {
                bool ret = _mapper.insert(mqp);
                if (ret == false)
                    return false;
            }
            _queues.insert(std::make_pair(qname, mqp));
            return true;
        }
        /// @brief 删除消息队列
        /// @param name 消息队列名称
        void deleteQueue(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _queues.find(name);
            if (it == _queues.end())
                return;
            _queues.erase(name);
            if (it->second->durable == true)
                _mapper.remove(name);
        }
        /// @brief 获取指定消息队列
        /// @param name 消息队列名称
        /// @return 消息队列对象指针
        MsgQueue::ptr selectQueue(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _queues.find(name);
            if (it == _queues.end())
                return MsgQueue::ptr();
            return it->second;
        }
        /// @brief 获取所有队列
        /// @return 消息队列映射表
        /// @see QueueMap
        QueueMap &allQueue()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _queues;
        }
        /// @brief 判断消息队列是否存在
        /// @param name 消息队列名称
        /// @return 存在则返回true 不存在返回false
        bool exists(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _queues.find(name);
            if (it == _queues.end())
                return false;
            return true;
        }
        /// @brief 获取消息队列数量
        /// @return 消息队列数量
        size_t size()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _queues.size();
        }
        /// @brief 清除消息队列
        void clear()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _mapper.removeTable();
            _queues.clear();
        }

    private:
        std::mutex _mutex;      ///< 互斥锁
        MsgQueueMapper _mapper; ///< 持久化消息队列管理类
        QueueMap _queues;       ///< 全部消息队列信息
    };
}