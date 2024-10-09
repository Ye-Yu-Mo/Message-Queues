/// @file consumer.h
/// @brief 消费者管理模块
///
/// 本文件定义了消费者及其管理类，用于管理队列中的消费者。
/// 提供了消费者的创建、移除、选择等功能，以及对消费者队列的管理。
/// 主要包括以下结构和类：
/// - Consumer: 表示单个消费者的结构
/// - QueueConsumer: 管理单个队列中的多个消费者
/// - ConsumerManager: 管理所有消费者队列

#pragma once
#include "../common/logger.hpp"
#include "../common/helper.hpp"
#include "../common/msg.pb.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>
#include <functional>

namespace XuMQ
{
    using ConsumerCallback = std::function<void(const std::string, const BasicProperties *, const std::string)>; ///< 消费者回调函数
    /// @struct Consumer
    /// @brief 消费者对象结构
    struct Consumer
    {
        using ptr = std::shared_ptr<Consumer>; ///< 消费者结构管理指针
        std::string tag;                       ///< 消费者标识
        std::string qname;                     ///< 消费者订阅的队列名称
        bool auto_ack;                         ///< 自动确认标志
        ConsumerCallback callback;             ///< 消费者回调函数

        /// @brief 无参构造函数
        Consumer() {}
        /// @brief 消费者构造函数
        /// @param ctag 消费者标识
        /// @param queue_name 消费者订阅的队列名称
        /// @param ack 自动确认标志
        /// @param cb 消费者回调函数
        Consumer(const std::string &ctag, const std::string &queue_name, bool ack, const ConsumerCallback &cb)
            : tag(ctag), qname(queue_name), auto_ack(ack), callback(cb) {}
    };

    /// @class QueueConsumer
    /// @brief 以队列为单元的消费者管理类
    class QueueConsumer
    {
    public:
        using ptr = std::shared_ptr<QueueConsumer>; ///< 消费者管理类
        /// @brief 构造函数
        /// @param qname 队列名称
        QueueConsumer(const std::string &qname)
            : _qname(qname), _rr_seq(0) {}
        /// @brief 创建一个消费者
        /// @param ctag 消费者标识
        /// @param queue_name 消费者订阅的队列名称
        /// @param ack 自动确认标志
        /// @param cb 消费者回调函数
        /// @return 消费者结构管理指针
        Consumer::ptr create(const std::string &ctag, const std::string &queue_name, bool ack, const ConsumerCallback &cb)
        {
            // 加锁
            std::unique_lock<std::mutex> lock(_mutex);
            // 判断消费者是否重复
            for (auto &consumer : _consumers)
            {
                if (consumer->tag == ctag)
                {
                    warn(logger, "消费者重复添加! 消费者标识: %s", ctag.c_str());
                    return Consumer::ptr();
                }
            }
            // 构造对象
            auto consumer = std::make_shared<Consumer>(ctag, queue_name, ack, cb);
            // 添加消费者
            _consumers.push_back(consumer);
            return consumer;
        }
        /// @brief 移除一个消费者
        /// @param ctag 消费者标识
        void remove(const std::string &ctag)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            for (auto it = _consumers.begin(); it != _consumers.end(); it++)
            {
                if (ctag == (*it)->tag)
                {
                    _consumers.erase(it);
                    return;
                }
            }
        }
        /// @brief 获取一个消费者
        /// @return 消费者结构管理指针
        Consumer::ptr choose()
        {
            // 加锁
            std::unique_lock<std::mutex> lock(_mutex);
            if (_consumers.size() == 0)
            {
                error(logger, "当前消费者队列为空!");
                return Consumer::ptr();
            }
            // 获取轮转到的下标
            int idx = _rr_seq++ % _consumers.size();
            // 返回
            return _consumers[idx];
        }
        /// @brief 判断消费者队列是否为空
        /// @return 为空返回true 不为空返回false
        bool empty()
        {
            // 加锁
            std::unique_lock<std::mutex> lock(_mutex);
            return _consumers.size() == 0;
        }
        /// @brief 判断消费者是否存在
        /// @param ctag 消费者标识
        /// @return 存在返回true 不存在返回false
        bool exists(const std::string &ctag)
        {
            // 加锁
            std::unique_lock<std::mutex> lock(_mutex);
            for (auto &consumer : _consumers)
                if (consumer->tag == ctag)
                    return true;
            return false;
        }
        /// @brief 清空
        void clear()
        {
            // 加锁
            std::unique_lock<std::mutex> lock(_mutex);
            _consumers.clear();
            _rr_seq = 0;
        }

    private:
        std::string _qname;                    ///< 队列名称
        std::mutex _mutex;                     ///< 互斥锁
        uint64_t _rr_seq;                      ///< 轮转序号
        std::vector<Consumer::ptr> _consumers; ///< 消费者管理数组
    };

    /// @class ConsumerManager
    /// @brief 消费者队列管理器
    class ConsumerManager
    {
    public:
        using ptr = std::shared_ptr<ConsumerManager>; ///< 消息队列管理器指针
        /// @brief 无参构造
        ConsumerManager() {}
        /// @brief 初始化消费者队列
        /// @param name 队列名称
        void initQueueConsumer(const std::string &name)
        {
            // 加锁
            std::unique_lock<std::mutex> lock(_mutex);
            // 判断重复
            auto it = _qconsumers.find(name);
            if (it != _qconsumers.end())
            {
                return;
            }
            // 新增
            auto qconsumers = std::make_shared<QueueConsumer>(name);
            _qconsumers.insert(std::make_pair(name, qconsumers));
        }
        /// @brief 销毁消费者队列
        /// @param name 队列名称
        void destroyQueueConsumer(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _qconsumers.erase(name);
        }
        /// @brief 向指定队列新增消费者
        /// @param ctag 消费者标识
        /// @param queue_name 队列名称
        /// @param ack 自动确认标志
        /// @param cb 消费者回调函数
        /// @return 消费者指针
        Consumer::ptr create(const std::string &ctag, const std::string &queue_name, bool ack, const ConsumerCallback &cb)
        {
            QueueConsumer::ptr qcp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                // 获取队列的消费者管理单元
                auto it = _qconsumers.find(queue_name);
                if (it == _qconsumers.end())
                {
                    warn(logger, "没有找到指定队列! 队列名称: %s", queue_name.c_str());
                    return Consumer::ptr();
                }
                qcp = it->second;
            }
            // 完成新建
            return qcp->create(ctag, queue_name, ack, cb);
        }
        /// @brief 移除队列的一个消费者
        /// @param ctag 消费者标识
        /// @param queue_name 队列名称
        void remove(const std::string &ctag, const std::string &queue_name)
        {
            QueueConsumer::ptr qcp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                // 获取队列的消费者管理单元
                auto it = _qconsumers.find(queue_name);
                if (it == _qconsumers.end())
                {
                    warn(logger, "没有找到指定队列! 队列名称: %s", queue_name.c_str());
                    return;
                }
                qcp = it->second;
            }
            qcp->remove(ctag);
        }
        /// @brief 获取指定队列的消费者
        /// @param queue_name 队列名称
        /// @return 消费者指针
        Consumer::ptr choose(const std::string &queue_name)
        {
            QueueConsumer::ptr qcp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                // 获取队列的消费者管理单元
                auto it = _qconsumers.find(queue_name);
                if (it == _qconsumers.end())
                {
                    warn(logger, "没有找到指定队列! 队列名称: %s", queue_name.c_str());
                    return Consumer::ptr();
                }
                qcp = it->second;
            }
            return qcp->choose();
        }

        /// @brief 判断指定队列是否为空
        /// @param queue_name 队列名称
        /// @return 为空返回true 不为空返回false
        bool empty(const std::string &queue_name)
        {
            QueueConsumer::ptr qcp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                // 获取队列的消费者管理单元
                auto it = _qconsumers.find(queue_name);
                if (it == _qconsumers.end())
                {
                    warn(logger, "没有找到指定队列! 队列名称: %s", queue_name.c_str());
                    return true;
                }
                qcp = it->second;
            }
            return qcp->empty();
        }
        /// @brief 判断队列中的消费者是否存在
        /// @param ctag 消费者标识
        /// @param queue_name 队列名称
        /// @return 存在返回true 不存在返回false
        bool exists(const std::string &ctag, const std::string &queue_name)
        {
            QueueConsumer::ptr qcp;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                // 获取队列的消费者管理单元
                auto it = _qconsumers.find(queue_name);
                if (it == _qconsumers.end())
                {
                    warn(logger, "没有找到指定队列! 队列名称: %s", queue_name.c_str());
                    return false;
                }
                qcp = it->second;
            }
            return qcp->exists(ctag);
        }
        /// @brief 清理
        void clear()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            QueueConsumer::ptr qcp;
            _qconsumers.clear();
        }

    private:
        std::mutex _mutex;
        std::unordered_map<std::string, QueueConsumer::ptr> _qconsumers;
    };
}