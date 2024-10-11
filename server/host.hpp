/**
 * @file host.hpp
 * @brief XuMQ 虚拟机模块定义
 *
 * 此模块实现了虚拟主机的功能，包括交换机和队列的管理、消息的发布与消费，以及绑定信息的管理。
 * 它负责维护消息队列、交换机和消息的持久化状态，并处理相关的消息传递逻辑。
 *
 *
 * 包含了消息队列、交换机和消息管理的相关类和方法。
 */

#pragma once
#include "exchange.hpp"
#include "queue.hpp"
#include "binding.hpp"
#include "message.hpp"

namespace XuMQ
{
    /// @class VirtualHost
    /// @brief 虚拟机模块
    class VirtualHost
    {
    public:
        using ptr = std::shared_ptr<VirtualHost>;
        /// @brief 虚拟机构造函数 恢复历史消息
        /// @param hname 虚拟机名称
        /// @param basedir 基础目录
        /// @param dbfile 数据库目录
        VirtualHost(const std::string hname, const std::string &basedir, const std::string &dbfile)
            : _emp(std::make_shared<ExchangeManager>(dbfile)),
              _mqmp(std::make_shared<MsgQueueManager>(dbfile)),
              _bmp(std::make_shared<BindingManager>(dbfile)),
              _mmp(std::make_shared<MessageManager>(basedir))

        {
            // 获取所有队列信息 通过队列信息恢复历史消息
            QueueMap qm = _mqmp->allQueue();
            for (auto &q : qm)
                _mmp->initQueueMessage(q.first);
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
            return _emp->declareExchange(name, type, durable, auto_delete, args);
        }
        /// @brief 删除交换机
        /// @param name 交换机名称
        void deleteExchange(const std::string &name)
        {
            // 先删除绑定信息
            _bmp->removeExchangeBindings(name);
            return _emp->deleteExchange(name);
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
                          const google::protobuf::Map<std::string, std::string> &qargs)
        {
            // 初始化队列消息句柄
            // 消息对立创建
            _mmp->initQueueMessage(qname);
            return _mqmp->declareQueue(qname, qdurable, qexclusive, qauto_delete, qargs);
        }
        /// @brief 删除消息队列
        /// @param name 消息队列名称
        void deleteQueue(const std::string &name)
        {
            // 删除队列消息
            _mmp->destroyQueueMessage(name);
            // 删除队列绑定信息
            _bmp->removeMsgQueueBindings(name);
            return _mqmp->deleteQueue(name);
        }

        /// @brief 添加绑定信息
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        /// @param key 绑定关键字
        /// @return 添加成功则返回true 失败返回false
        /// @note 当交换机和消息队列的持久化标志都为true时 绑定信息持久化标志为true才有意义
        bool bind(const std::string &ename, const std::string &qname, const std::string &key)
        {
            Exchange::ptr ep = _emp->selectExchange(ename);
            if (ep.get() == nullptr)
            {
                error(logger, "队列绑定失败, 交换机 %s 不存在", ename.c_str());
                return false;
            }
            MsgQueue::ptr mqp = _mqmp->selectQueue(qname);
            if (mqp.get() == nullptr)
            {
                error(logger, "队列绑定失败, 队列 %s 不存在", qname.c_str());
                return false;
            }
            return _bmp->bind(ename, qname, key, ep->durable & mqp->durable);
        }
        /// @brief 解除绑定信息
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        void unbind(const std::string &ename, const std::string &qname)
        {
            return _bmp->unbind(ename, qname);
        }
        /// @brief 获取交换机绑定信息
        /// @param ename 交换机名称
        /// @return 消息队列绑定映射表 @see MsgQueueBindingMap
        MsgQueueBindingMap exchangeBindings(const std::string &ename)
        {
            return _bmp->getExchangeBindings(ename);
        }

        /// @brief 向指定队列插入新消息
        /// @param qname 消息队列名称
        /// @param bp 消息属性
        /// @param body 消息主体
        /// @return 插入成功返回true 失败返回false
        bool basicPublish(const std::string &qname, BasicProperties *bp, const std::string &body)
        {
            MsgQueue::ptr mqp = _mqmp->selectQueue(qname);
            if (mqp.get() == nullptr)
            {
                error(logger, "队列绑定失败, 队列 %s 不存在", qname.c_str());
                return false;
            }
            return _mmp->insert(qname, bp, body, mqp->durable);
        }
        /// @brief 获取队头消息
        /// @param qname 消息队列名称
        /// @return 消息指针
        MessagePtr basicConsume(const std::string &qname)
        {
            return _mmp->front(qname);
        }
        /// @brief 应答消息
        /// @param qname 消息队列名称
        /// @param msg_id 消息id
        void basicAck(const std::string &qname, const std::string &msg_id)
        {
            _mmp->ack(qname, msg_id);
        }

        /// @brief 获取指定交换机句柄
        /// @param ename 交换机名称
        /// @return 交换机句柄
        Exchange::ptr selectExchange(const std::string& ename)
        {
            return _emp->selectExchange(ename);
        }

        /// @brief 清理
        void clear()
        {
            _emp->clear();
            _mqmp->clear();
            _bmp->clear();
            _mmp->clear();
        }

        /// @brief 判断交换机是否存在
        /// @param name 交换机名称
        /// @return 存在返回true 不存在返回false
        bool existsExchange(const std::string &name)
        {
            return _emp->exists(name);
        }

        /// @brief 判断队列是否存在
        /// @param name 队列名称
        /// @return 存在返回true 不存在返回false
        bool existsQueue(const std::string &name)
        {
            return _mqmp->exists(name);
        }

        /// @brief 判断绑定信息是否存在
        /// @param ename 交换机名称
        /// @param qname 队列名称
        /// @return 存在返回true 不存在返回false
        bool existsBinding(const std::string &ename, const std::string &qname)
        {
            return _bmp->exists(ename, qname);
        }

    private:
        std::string _host_name;
        ExchangeManager::ptr _emp;  ///< 交换机管理指针
        MsgQueueManager::ptr _mqmp; ///<  消息队列管理指针
        BindingManager::ptr _bmp;   ///< 绑定信息管理指针
        MessageManager::ptr _mmp;   ///< 消息管理指针
    };
}