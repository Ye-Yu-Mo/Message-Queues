/// @file consumer.hpp
/// @brief 客户端消费者模块
///
/// 本文件定义了消费者，用于标识客户但中的消费者。

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
    using ConsumerCallback = std::function<void(const std::string &, const BasicProperties *, const std::string &)>; ///< 消费者回调函数
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
}