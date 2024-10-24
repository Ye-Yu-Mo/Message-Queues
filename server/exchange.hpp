/**
 * @file exchange.hpp
 * @brief 交换机管理模块，定义了交换机的结构、持久化以及内存管理功能
 *
 * 该模块定义了交换机的基本结构（Exchange），以及交换机的持久化类（ExchangeMapper）
 * 和内存管理类（ExchangeManager）。实现了交换机的声明、删除、持久化等操作，并通过SQLite
 * 数据库实现数据的持久化。
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
    /// @struct Exchange
    /// @brief 交换机结构体对象
    struct Exchange
    {
        using ptr = std::shared_ptr<Exchange>;             ///< 使用智能指针管理交换机对象
        std::string name;                                  ///< 交换机名称
        ExchangeType type;                                 ///< 交换机类型
        bool durable;                                      ///< 数据持久化标志
        bool auto_delete;                                  ///< 自动删除标志
        google::protobuf::Map<std::string, std::string> args; ///< 其他参数

        /// @brief 无参构造
        Exchange() {}
        /// @brief 交换机结构构造函数
        /// @param ename 交换机名称
        /// @param etype 交换机类型
        /// @param edurable 数据持久化标志
        /// @param eauto_delete 自动删除标志
        /// @param eargs 其他参数
        Exchange(const std::string &ename,
                 ExchangeType etype,
                 bool edurable,
                 bool eauto_delete,
                 const google::protobuf::Map<std::string, std::string> &eargs)
            : name(ename), type(etype), durable(edurable), auto_delete(eauto_delete), args(eargs)
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
                args[key] = value;
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
    using ExchangeMap = std::unordered_map<std::string, Exchange::ptr>; ///< 交换机映射表 交换机名称->交换机对象指针
    /// @class ExchangerMapper
    /// @brief 交换机持久化管理类 将数据存储在sqlite数据库中
    class ExchangeMapper
    {
    public:
        /// @brief 交换机持久化管理类 构造函数
        /// @param dbfile 数据库名称
        /// @note 如果数据库不存在则自动创建
        ExchangeMapper(const std::string &dbfile)
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
            const char *CREATE_TABLE = "create table if not exists exchange_table(name varchar(32) primary key,\
            type tinyint, durable tinyint,\
            auto_delete tinyint, args varchar(128));";
            bool ret = _sql_helper.exec(CREATE_TABLE, nullptr, nullptr);
            if (ret == false)
            {
                fatal(logger, "创建交换机数据库表失败!");
                abort();
            }
        }
        /// @brief 移除一张表
        void removeTable()
        {
            const char *DROP_TABLE = "drop table if exists exchange_table;";
            bool ret = _sql_helper.exec(DROP_TABLE, nullptr, nullptr);
            if (ret == false)
            {
                fatal(logger, "删除交换机数据库表失败!");
                abort();
            }
        }
        /// @brief 新增一个交换机
        /// @param exchange 交换机对象指针
        /// @see Exchange::ptr
        /// @return 插入成功返回true 插入失败输出日志并返回false
        bool insert(Exchange::ptr &exchange)
        {
            const char *INSERT_SQL = "insert into exchange_table values('%s', %d, %d, %d, '%s');";
            char sql_str[4096] = {0};
            sprintf(sql_str, INSERT_SQL, exchange->name.c_str(), exchange->type, exchange->durable,
                    exchange->auto_delete, exchange->getArgs().c_str());
            bool ret = _sql_helper.exec(sql_str, nullptr, nullptr);
            if (ret == false)
            {
                error(logger, "数据库: 插入交换机失败!");
                return false;
            }
            return true;
        }
        /// @brief 移除一个交换机
        /// @param name 交换机名称
        /// @return 删除成功返回true 删除失败输出日志并返回false
        bool remove(const std::string &name)
        {
            const char *DELETE_SQL = "delete from exchange_table where name = '%s';";
            char sql_str[4096] = {0};
            sprintf(sql_str, DELETE_SQL, name.c_str());
            bool ret = _sql_helper.exec(sql_str, nullptr, nullptr);
            if (ret == false)
            {
                error(logger, "数据库: 删除交换机失败!");
                return false;
            }
            return true;
        }
        /// @brief 获取所有交换机 从数据库加载到内存
        /// @return 一张映射表 交换机名称->交换机对象指针
        /// @see Exchange::ptr ExchangeManager
        ExchangeMap recovery()
        {
            ExchangeMap result;
            const char *SELECT_SQL = "select * from exchange_table;";
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
            ExchangeMap *result = (ExchangeMap *)arg;
            auto exp = std::make_shared<Exchange>();
            exp->name = row[0];
            exp->type = (XuMQ::ExchangeType)std::stoi(row[1]);
            exp->durable = (bool)std::stoi(row[2]);
            exp->auto_delete = (bool)std::stoi(row[3]);
            if (row[4])
                exp->setArgs(row[4]);
            result->insert(std::make_pair(exp->name, exp));
            return 0;
        }

    private:
        SqliteHelper _sql_helper; ///< 数据库操作对象
    };
    /// @class ExchangeManager
    /// @brief 交换机数据内存管理类
    class ExchangeManager
    {
    public:
        using ptr = std::shared_ptr<ExchangeManager>; ///< 交换机数据内存管理指针
        /// @brief 交换机数据内存管理类 构造函数 从数据库中恢复数据
        /// @param dbfile 数据库名称
        ExchangeManager(const std::string &dbfile)
            : _mapper(dbfile)
        {
            _exchanges = _mapper.recovery();
        }
        /// @brief 声明交换机
        /// @param name 交换机名称
        /// @param type 交换机类型
        /// @param durable 数据持久化标志
        /// @param auto_delete 自动删除标志
        /// @param args 其他参数
        /// @return 声明成功返回true 失败返回false
        bool declareExchange(const std::string &name,
                             ExchangeType type,
                             bool durable,
                             bool auto_delete,
                             const google::protobuf::Map<std::string, std::string> &args)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _exchanges.find(name);
            if (it != _exchanges.end())
                return true;
            auto exp = std::make_shared<Exchange>(name, type, durable, auto_delete, args);
            if (durable)
            {
                bool ret = _mapper.insert(exp);
                if (ret == false)
                    return false;
            }
            _exchanges.insert(std::make_pair(name, exp));
            return true;
        }
        /// @brief 删除交换机
        /// @param name 交换机名称
        void deleteExchange(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _exchanges.find(name);
            if (it == _exchanges.end())
                return;
            _exchanges.erase(name);
            if (it->second->durable == true)
                _mapper.remove(name);
        }
        /// @brief 获取指定交换机
        /// @param name 交换机名称
        /// @return 交换机对象指针
        Exchange::ptr selectExchange(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _exchanges.find(name);
            if (it == _exchanges.end())
                return Exchange::ptr();
            return it->second;
        }
        /// @brief 判断交换机是否存在
        /// @param name 交换机名称
        /// @return true表示交换机存在 flase表示交换机不存在
        bool exists(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _exchanges.find(name);
            if (it == _exchanges.end())
                return false;
            return true;
        }
        /// @brief 清除所有交换机数据
        void clear()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _mapper.removeTable();
            _exchanges.clear();
        }
        /// @brief 获取交换机数量
        /// @return 交换机数量
        size_t size()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _exchanges.size();
        }

    private:
        std::mutex _mutex;      ///< 互斥锁
        ExchangeMapper _mapper; ///< 持久化交换机管理类
        ExchangeMap _exchanges; ///< 全部交换机信息
    };
}