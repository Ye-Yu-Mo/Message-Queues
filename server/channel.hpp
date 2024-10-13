/**
 * @file channel.hpp
 * @brief 信道与信道管理模块的头文件，包含了信道的声明和其管理类的定义。
 * 
 * 该文件定义了消息队列系统中信道 (Channel) 及信道管理 (ChannelManager) 的类，主要用于处理信道相关的操作
 * 例如声明队列、发布消息、消费者回调等。依赖于 Protobuf 和 Muduo 库，并使用了自定义的日志模块和线程池。
 * 
 * - 消息队列系统模块
 * - 基于 TCP 和 Protobuf 的消息通信
 * - 支持多线程处理和消费者管理
 */

#pragma once
#include "muduo/net/TcpConnection.h"
#include "muduo/protobuf/codec.h"
#include "muduo/protobuf/dispatcher.h"
#include "muduo/base/Atomic.h"
#include "muduo/base/Types.h"

#include "../common/logger.hpp"
#include "../common/helper.hpp"
#include "../common/msg.pb.h"
#include "../common/protocol.pb.h"
#include "../common/threadpool.hpp"
#include <google/protobuf/map.h>
#include "consumer.hpp"
#include "host.hpp"
#include "route.hpp"

namespace XuMQ
{
    using ProtobufCodecPtr = std::shared_ptr<ProtobufCodec>;                   ///< 协议处理句柄
    using openChannelRequestPtr = std::shared_ptr<openChannelRequest>;         ///< 打开信道请求
    using closeChannelRequestPtr = std::shared_ptr<closeChannelRequest>;       ///< 关闭信道请求
    using declareExchangeRequestPtr = std::shared_ptr<declareExchangeRequest>; ///< 声明交换机请求
    using deleteExchangeRequestPtr = std::shared_ptr<deleteExchangeRequest>;   ///< 删除交换机请求
    using declareQueueRequestPtr = std::shared_ptr<declareQueueRequest>;       ///< 声明队列请求
    using deleteQueueRequestPtr = std::shared_ptr<deleteQueueRequest>;         ///< 删除队列请求
    using queueBindRequestPtr = std::shared_ptr<queueBindRequest>;             ///< 绑定请求
    using queueUnBindRequestPtr = std::shared_ptr<queueUnBindRequest>;         ///< 解除绑定请求
    using basicPublishRequestPtr = std::shared_ptr<basicPublishRequest>;       ///< 消息发布请求
    using basicAckRequestPtr = std::shared_ptr<basicAckRequest>;               ///< 消息应答请求
    using basicCancelRequestPtr = std::shared_ptr<basicCancelRequest>;         ///< 取消订阅请求
    using basicConsumeRequestPtr = std::shared_ptr<basicConsumeRequest>;       ///< 取消订阅请求
    /// @class Channel
    /// @brief 信道类
    class Channel
    {
    public:
        using ptr = std::shared_ptr<Channel>;
        /// @brief 信道构造函数
        /// @param id 信道id
        /// @param host 虚拟机
        /// @param cmp 消费者管理句柄
        /// @param codec 协议处理句柄
        /// @param conn muduo连接管理句柄
        /// @param pool 线程池管理句柄
        Channel(const std::string &id, const VirtualHost::ptr &host, const ConsumerManager::ptr &cmp,
                const ProtobufCodecPtr &codec, const muduo::net::TcpConnectionPtr &conn, const threadpool::ptr &pool)
            : _cid(id), _conn(conn), _codec(codec), _cmp(cmp), _host(host), _pool(pool) {}
        /// @brief 析构函数
        ~Channel()
        {
            if (_consumer.get() != nullptr)
                _cmp->remove(_consumer->tag, _consumer->qname);
        }
        /// @brief 声明交换机请求处理函数
        /// @param req 声明交换机请求
        void declareExchange(const declareExchangeRequestPtr &req)
        {
            bool ret = _host->declareExchange(req->exchange_name(), req->exchange_type(),
                                              req->durable(), req->auto_delete(), req->args());
            basicRespFunc(ret, req->rid(), req->cid());
        }
        /// @brief 删除交换机请求处理函数
        /// @param req 删除交换机请求
        void deleteExchange(const deleteExchangeRequestPtr &req)
        {
            _host->deleteExchange(req->exchange_name());
            basicRespFunc(true, req->rid(), req->cid());
        }
        /// @brief 声明队列请求处理函数
        /// @param req 声明队列请求
        void declareQueue(const declareQueueRequestPtr &req)
        {
            bool ret = _host->declareQueue(req->queue_name(), req->durable(),
                                           req->exclusive(), req->auto_delete(), req->args());
            if (ret == false)
                basicRespFunc(ret, req->rid(), req->cid());
            debug(logger, "声明队列成功 队列名称为%s", req->queue_name().c_str());
            _cmp->initQueueConsumer(req->queue_name()); // 初始化队列消费者管理句柄
            basicRespFunc(ret, req->rid(), req->cid());
        }
        /// @brief 删除队列请求处理函数
        /// @param req 删除队列请求
        void deleteQueue(const deleteQueueRequestPtr &req)
        {
            _cmp->destroyQueueConsumer(req->queue_name());
            _host->deleteQueue(req->queue_name());
            basicRespFunc(true, req->rid(), req->cid());
        }
        /// @brief 队列绑定请求处理函数
        /// @param req 队列绑定请求
        void queueBind(const queueBindRequestPtr &req)
        {
            bool ret = _host->bind(req->exchange_name(), req->queue_name(), req->binding_key());
            basicRespFunc(ret, req->rid(), req->cid());
        }
        /// @brief 队列解绑请求处理函数
        /// @param req 队列解绑请求
        void queueUnBind(const queueUnBindRequestPtr &req)
        {
            _host->unbind(req->exchange_name(), req->queue_name());
            basicRespFunc(true, req->rid(), req->cid());
        }
        /// @brief 消息发布请求处理函数
        /// @param req 消息发布请求
        void basicPublish(const basicPublishRequestPtr &req)
        {
            // 获取交换机
            Exchange::ptr ep = _host->selectExchange(req->exchange_name());
            if (ep.get() == nullptr)
                basicRespFunc(false, req->rid(), req->cid());
            // 获取指定交换机的绑定信息
            MsgQueueBindingMap mqbm = _host->exchangeBindings(req->exchange_name());
            BasicProperties *properties = nullptr;
            std::string routing_key;

            if (req->has_properties())
            {
                properties = req->mutable_properties();
                routing_key = properties->routing_key();
            }
            for (auto &binding : mqbm)
            {
                // 交换路由 找到对应的队列
                if (Router::route(ep->type, routing_key, binding.second->binding_key))
                {
                    // 将消息添加到队列中
                    _host->basicPublish(binding.first, properties, req->body());
                    // 向线程池中添加一个消息消费任务(向指定队列的订阅者推送消息)
                    auto task = std::bind(&Channel::consume, this, binding.first);
                    _pool->push(task);
                }
                basicRespFunc(true, req->rid(), req->cid());
            }
        }
        /// @brief 确认消息请求处理函数
        /// @param req 确认消息请求
        void basicAck(const basicAckRequestPtr &req)
        {
            _host->basicAck(req->queue_name(), req->msg_id());
            basicRespFunc(true, req->rid(), req->cid());
        }
        /// @brief 订阅消息请求处理函数
        /// @param req 订阅消息请求
        void basicConsume(const basicConsumeRequestPtr &req)
        {
            // 判断队列是否存在
            bool ret = _host->existsQueue(req->queue_name());
            if (ret == false)
                basicRespFunc(false, req->rid(), req->cid());
            // 创建队列消费者
            auto cb = std::bind(&Channel::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            // 创建消费者之后 信道的角色就是消费者
            _consumer = _cmp->create(req->consumer_tag(), req->queue_name(), req->auto_ack(), cb);
            if(_consumer==nullptr)
            {
                fatal(logger,"消费者创建失败！");
            }
            else
            {
                info(logger,"消费者创建成功！");
            }
            basicRespFunc(true, req->rid(), req->cid());
        }
        /// @brief 取消订阅请求处理函数
        /// @param req 取消订阅请求
        void basicCancel(const basicCancelRequestPtr &req)
        {
            _cmp->remove(req->consumer_tag(), req->queue_name());
            basicRespFunc(true, req->rid(), req->cid());
        }

    private:
        /// @brief 基础响应发送函数
        /// @param ok 响应结果
        /// @param rid 信道id
        /// @param cid 请求id
        void basicRespFunc(bool ok, const std::string &rid, const std::string &cid)
        {
            basicResponse resp;
            resp.set_rid(rid);
            resp.set_cid(cid);
            resp.set_ok(ok);
            _codec->send(_conn, resp);
        }
        /// @brief 消费调用函数
        /// @param qname 队列名称
        void consume(const std::string &qname)
        {
            MessagePtr mp = _host->basicConsume(qname);
            if (mp.get() == nullptr)
            {
                error(logger, "消费任务失败, 指定队列中没有消息: %s", qname.c_str());
                return;
            }
            Consumer::ptr cp = _cmp->choose(qname);
            if (cp.get() == nullptr)
            {
                error(logger, "消费任务失败, 指定队列中没有消费者: %s", qname.c_str());
                return;
            }
            cp->callback(cp->tag, mp->mutable_payload()->mutable_properties(), mp->payload().body());
            if (cp->auto_ack == true)
                _host->basicAck(qname, mp->payload().properties().id());
        }
        /// @brief 消费者回调函数
        /// @param tag 消费者标识
        /// @param bp 消息属性
        /// @param body 消息主体
        void callback(const std::string &tag, const BasicProperties *bp, const std::string &body)
        {
            basicConsumeResponse resp;
            resp.set_cid(_cid);
            resp.set_body(body);
            resp.set_consumer_tag(tag);
            if (bp)
            {
                resp.mutable_properties()->set_id(bp->id());
                resp.mutable_properties()->set_delivery_mode(bp->delivery_mode());
                resp.mutable_properties()->set_routing_key(bp->routing_key());
            }
            _codec->send(_conn, resp);
        }

    private:
        std::string _cid;                   ///< 信道id
        Consumer::ptr _consumer;            ///< 消费者对象
        muduo::net::TcpConnectionPtr _conn; ///< muduo连接管理句柄
        ProtobufCodecPtr _codec;            ///< 协议处理句柄
        ConsumerManager::ptr _cmp;          ///< 消费者管理句柄
        VirtualHost::ptr _host;             ///< 虚拟机
        threadpool::ptr _pool;              ///< 线程池
    };
    /// @class ChannelManager
    /// @brief 信道管理类
    class ChannelManager
    {
    public:
        using ptr = std::shared_ptr<ChannelManager>; ///< 信道管理句柄
        
        /// @brief 构造函数
        ChannelManager() {}
        /// @brief 打开一个信道
        /// @param id 信道id
        /// @param host 虚拟机
        /// @param cmp 消费者管理句柄
        /// @param codec 协议处理句柄
        /// @param conn muduo连接管理句柄
        /// @param pool 线程池管理句柄
        /// @return 成功返回 true 失败返回 false
        bool openChannel(const std::string &id, const VirtualHost::ptr &host, const ConsumerManager::ptr &cmp,
                         const ProtobufCodecPtr &codec, const muduo::net::TcpConnectionPtr &conn, const threadpool::ptr &pool)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _channels.find(id);
            if (it != _channels.end())
                return false;
            auto channel = std::make_shared<Channel>(id, host, cmp, codec, conn, pool);
            _channels.insert(std::make_pair(id, channel));
            return true;
        }
        /// @brief 关闭信道
        /// @param id 信道id
        void closeChannel(const std::string &id)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _channels.erase(id);
        }
        /// @brief 获取信道句柄
        /// @param id 信道id
        /// @return 信道管理句柄
        Channel::ptr getChannel(const std::string &id)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _channels.find(id);
            if (it == _channels.end())
                return Channel::ptr();
            return it->second;
        }

    private:
        std::mutex _mutex; ///< 互斥锁
        std::unordered_map<std::string, Channel::ptr> _channels; ///< 信道id到信道管理句柄的映射表
    };
}