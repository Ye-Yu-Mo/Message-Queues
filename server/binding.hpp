/**
 * @file binding.hpp
 * @brief 该文件包含消息队列绑定信息的管理类和结构体定义。
 *
 * 本模块实现了消息队列与交换机的绑定信息管理，包括持久化管理和内存管理。
 *
 * 1. 该模块主要负责消息队列与交换机之间的绑定关系的维护。
 * 2. 提供了对绑定信息的增删查改接口。
 * 3. 支持将绑定信息持久化到 SQLite 数据库中，以便于重启后恢复状态。
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
    /// @struct Binding
    /// @brief 绑定信息结构体
    struct Binding
    {
        using ptr = std::shared_ptr<Binding>; ///< 绑定信息指针
        std::string exchange_name;            ///< 交换机名称
        std::string msgqueue_name;            ///< 消息队列名称
        std::string binding_key;              ///< 绑定关键字
        /// @brief 无参构造
        Binding() {}
        /// @brief 绑定信息结构体 构造函数
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        /// @param key 绑定关键字
        Binding(const std::string &ename, const std::string &qname, const std::string &key)
            : exchange_name(ename), msgqueue_name(qname), binding_key(key) {}
    };
    /// @brief 消息队列绑定映射表----消息队列->绑定信息的映射表
    /// @note 消息队列与绑定信息是一一对应的, 方便从队列名查找绑定信息
    /// first是消息队列名称 second是绑定信息指针
    using MsgQueueBindingMap = std::unordered_map<std::string, Binding::ptr>;
    /// @brief 绑定映射表----交换机->消息队列绑定映射表的映射表
    /// @note 一个交换机可能会有多个队列的绑定信息
    /// 从交换机名称找到其关联的所有的消息队列和对应的绑定信息
    /// 这一张表包含了所有的交换机, 消息队列, 绑定信息
    /// 以交换机为单元进行区分 first是交换机名称 second是消息队列绑定映射表
    /// @deprecated
    /// 已弃用
    /// std::unordered_map<std::string, Binding::ptr>; 队列->绑定信息
    /// std::unordered_map<std::string, Binding::ptr>; 交换机->绑定信息
    /// 删除交换机绑定信息时, 需要删除交换机->绑定信息中的数据
    /// 额外需要遍历队列->绑定信息中绑定信息的交换机名称 再进行释放 效率极低
    using BindingMap = std::unordered_map<std::string, MsgQueueBindingMap>;
    /// @class BindingMapper
    /// @brief 绑定信息持久化管理类
    class BindingMapper
    {
    public:
        /// @brief 绑定信息持久化管理类 构造函数
        /// @param dbfile 数据库名称
        /// @note 如果数据库不存在则自动创建
        BindingMapper(const std::string &dbfile)
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
            const char *CREATE_TABLE = "create table if not exists binding_table(exchange_name varchar(32), msgqueue_name varchar(32), binding_key varchar(128));";
            bool ret = _sql_helper.exec(CREATE_TABLE, nullptr, nullptr);
            if (ret == false)
            {
                fatal(logger, "创建绑定信息数据库表失败!");
                abort();
            }
        }
        /// @brief 移除一张表
        void removeTable()
        {
            const char *DROP_TABLE = "drop table if exists binding_table;";
            bool ret = _sql_helper.exec(DROP_TABLE, nullptr, nullptr);
            if (ret == false)
            {
                fatal(logger, "删除绑定信息数据库表失败!");
                abort();
            }
        }
        /// @brief 新增一个绑定信息
        /// @param binding 消息绑定信息指针
        /// @return 新增成功返回true 失败返回false并输出日志
        bool insert(Binding::ptr &binding)
        {
            const char *INSERT_SQL = "insert into binding_table values('%s', '%s', '%s');";
            char sql_str[4096] = {0};
            sprintf(sql_str, INSERT_SQL, binding->exchange_name.c_str(),
                    binding->msgqueue_name.c_str(), binding->binding_key.c_str());
            bool ret = _sql_helper.exec(sql_str, nullptr, nullptr);
            if (ret == false)
            {
                error(logger, "数据库: 插入绑定信息失败!");
                return false;
            }
            return true;
        }
        /// @brief 移除一个绑定信息
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        /// @return 删除成功返回true 删除失败输出日志并返回false
        bool remove(const std::string &ename, const std::string &qname)
        {
            const char *DELETE_SQL = "delete from binding_table where exchange_name = '%s' \
            and msgqueue_name = '%s';";
            char sql_str[4096] = {0};
            sprintf(sql_str, DELETE_SQL, ename.c_str(), qname.c_str());
            bool ret = _sql_helper.exec(sql_str, nullptr, nullptr);
            if (ret == false)
            {
                error(logger, "数据库: 删除绑定信息失败!");
                return false;
            }
            return true;
        }
        /// @brief 移除交换机绑定信息
        /// @param ename 交换机名称
        /// @return 删除成功返回true 删除失败输出日志并返回false
        bool removeExchangeBindings(const std::string &ename)
        {
            const char *DELETE_SQL = "delete from binding_table where exchange_name = '%s';";
            char sql_str[4096] = {0};
            sprintf(sql_str, DELETE_SQL, ename.c_str());
            bool ret = _sql_helper.exec(sql_str, nullptr, nullptr);
            if (ret == false)
            {
                error(logger, "数据库: 删除交换机绑定信息失败!");
                return false;
            }
            return true;
        }
        /// @brief 移除消息队列绑定信息
        /// @param ename 消息队列名称
        /// @return 删除成功返回true 删除失败输出日志并返回false
        bool removeQueueBindings(const std::string &qname)
        {
            const char *DELETE_SQL = "delete from binding_table where msgqueue_name = '%s';";
            char sql_str[4096] = {0};
            sprintf(sql_str, DELETE_SQL, qname.c_str());
            bool ret = _sql_helper.exec(sql_str, nullptr, nullptr);
            if (ret == false)
            {
                error(logger, "数据库: 删除消息队列绑定信息失败!");
                return false;
            }
            return true;
        }
        /// @brief 获取所有绑定信息 从数据库加载到内存
        /// @return 一张映射表 交换机->消息队列绑定映射表的映射表
        /// @see BindingMap
        BindingMap recovery()
        {
            BindingMap result;
            const char *SELECT_SQL = "select * from binding_table;";
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
            BindingMap *result = (BindingMap *)arg;
            Binding::ptr bp = std::make_shared<Binding>(row[0], row[1], row[2]);
            MsgQueueBindingMap &qmap = (*result)[bp->exchange_name];
            /// @note
            /// 防止交换机相关绑定信息已经存在 不能直接创建队列映射 会覆盖历史数据
            /// 先获取交换机的映射对象 向其中添加数据
            /// 若交换机没有映射信息 使用引用则会自动创建
            qmap.insert(std::make_pair(bp->msgqueue_name, bp));
            return 0;
        }

    private:
        SqliteHelper _sql_helper; ///< 数据库操作对象
    };
    /// @class BindingManager
    /// @brief 绑定信息内存管理类
    class BindingManager
    {
    public:
        using ptr = std::shared_ptr<BindingManager>; ///< 绑定信息内存管理类指针
        /// @brief 绑定信息数据内存管理类 构造函数 从数据库中恢复数据
        /// @param dbfile 数据库名称
        BindingManager(const std::string &dbfile) : _mapper(dbfile) {
            _bindings = _mapper.recovery();
        }
        /// @brief 添加绑定信息
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        /// @param key 绑定关键字
        /// @param durable 持久化标志
        /// @return 添加成功则返回true 失败返回false
        /// @note 当交换机和消息队列的持久化标志都为true时 绑定信息持久化标志为true才有意义
        bool bind(const std::string &ename, const std::string &qname, const std::string &key, bool durable)
        {
            // 加锁 构造一个队列信息绑定对象 添加映射关系
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _bindings.find(ename);
            if (it != _bindings.end() && it->second.find(qname) != it->second.end()) // 绑定信息已经存在
                return true;
            Binding::ptr bp = std::make_shared<Binding>(ename, qname, key);
            if (durable)
            {
                bool ret = _mapper.insert(bp);
                if (ret == false)
                    return false;
            }
            auto &qbmap = _bindings[ename];
            qbmap.insert(std::make_pair(qname, bp));
            return true;
        }
        /// @brief 解除绑定信息
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        void unbind(const std::string &ename, const std::string &qname)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto eit = _bindings.find(ename);
            if (eit == _bindings.end()) // 没有交换机的绑定信息
                return;
            auto qit = eit->second.find(qname); // 没有交换机对应队列的绑定信息
            if (qit == eit->second.end())
                return;
            _mapper.remove(ename, qname);
            _bindings[ename].erase(qname);
        }
        /// @brief 移除指定交换机的所有绑定信息
        /// @param ename 交换机名称
        void removeExchangeBindings(const std::string &ename)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _mapper.removeExchangeBindings(ename);
            _bindings.erase(ename);
        }
        /// @brief 移除指定消息队列的所有绑定信息
        /// @param qname 消息队列名称
        void removeMsgQueueBindings(const std::string &qname)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _mapper.removeQueueBindings(qname);
            for (auto &binding : _bindings) // 遍历所有交换机
                binding.second.erase(qname);
        }
        /// @brief 获取指定交换机的绑定信息
        /// @param ename 交换机名称
        /// @return 消息队列绑定映射表 @see MsgQueueBindingMap
        MsgQueueBindingMap getExchangeBindings(const std::string &ename)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto eit = _bindings.find(ename);
            if (eit == _bindings.end())
                return MsgQueueBindingMap();
            return eit->second;
        }

        /// @brief 获取绑定信息
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        /// @return 绑定信息指针 @see Binding::ptr
        Binding::ptr getBinding(const std::string &ename, const std::string &qname)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto eit = _bindings.find(ename);
            if (eit == _bindings.end())
                return Binding::ptr();
            auto qit = eit->second.find(qname);
            if (qit == eit->second.end())
                return Binding::ptr();
            return qit->second;
        }
        /// @brief 判断绑定信息是否存在
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        /// @return 存在则返回true 不存在返回false
        bool exists(const std::string &ename, const std::string &qname)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto eit = _bindings.find(ename);
            if (eit == _bindings.end())
                return false;
            auto qit = eit->second.find(qname);
            if (qit == eit->second.end())
                return false;
            return true;
        }
        /// @brief 获取绑定信息数量
        /// @return 绑定信息数量
        size_t size()
        {
            size_t total_size = 0;
            std::unique_lock<std::mutex> lock(_mutex);
            for (auto &it : _bindings)
                total_size += it.second.size();
            return total_size;
        }
        /// @brief 清除绑定信息
        void clear()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _mapper.removeTable();
            _bindings.clear();
        }

    private:
        std::mutex _mutex;     ///< 互斥锁
        BindingMapper _mapper; ///< 绑定信息持久化管理类
        BindingMap _bindings;  ///< 绑定映射表
    };
}