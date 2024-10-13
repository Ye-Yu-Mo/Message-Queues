/**
 * @file channel.hpp
 * @brief 信道与信道管理模块的头文件，包含了信道的声明和其管理类的定义。
 *
 * 该文件定义了服务端中信道 (Channel) 及信道管理 (ChannelManager) 的类，主要用于传递操作
 *
 */

#pragma once
#include "muduo/net/TcpConnection.h"
#include "muduo/protobuf/codec.h"
#include "muduo/protobuf/dispatcher.h"
#include "../common/logger.hpp"
#include "../common/helper.hpp"
#include "../common/msg.pb.h"
#include "../common/protocol.pb.h"
#include "consumer.hpp"
#include <mutex>
#include <condition_variable>

namespace XuMQ
{
    using MessagePtr = std::shared_ptr<google::protobuf::Message>;         ///< 消息句柄
    using ProtobufCodecPtr = std::shared_ptr<ProtobufCodec>;               ///< 协议处理句柄
    using basicConsumeResponsePtr = std::shared_ptr<basicConsumeResponse>; ///< 消费响应句柄
    using basicResponsePtr = std::shared_ptr<basicResponse>;               ///< 其他响应句柄
    /// @class Channel
    /// @brief 客户端信道类
    class Channel
    {
    public:
        using ptr = std::shared_ptr<Channel>; ///< 信道句柄
        /// @brief 构造函数
        /// @param conn muduo连接句柄
        /// @param codec 协议处理句柄
        Channel(const muduo::net::TcpConnectionPtr &conn, const ProtobufCodecPtr &codec)
            : _cid(UUIDHelper::uuid()), _conn(conn), _codec(codec) {}
        ~Channel()
        {
            basicCancel();
        }
        /// @brief 创建信道请求
        /// @return 成功返回true 失败返回false
        bool openChannel()
        {
            std::string rid = UUIDHelper::uuid();
            openChannelRequest req;
            req.set_rid(rid);
            req.set_cid(_cid);
            _codec->send(_conn, req);
            basicResponsePtr resp = waitResponse(rid);
            return resp->ok();
        }
        /// @brief 关闭信道请求
        void closeChannel()
        {
            std::string rid = UUIDHelper::uuid();
            closeChannelRequest req;
            req.set_rid(rid);
            req.set_cid(_cid);
            _codec->send(_conn, req);
            waitResponse(rid);
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
                             google::protobuf::Map<std::string, std::string> &args)
        {
            // 构造一个声明交换机的请求对象
            declareExchangeRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_exchange_name(name);
            req.set_exchange_type(type);
            req.set_durable(durable);
            req.set_auto_delete(auto_delete);
            req.mutable_args()->swap(args);
            // 发送请求
            _codec->send(_conn, req);
            // 等待服务器响应
            basicResponsePtr resp = waitResponse(rid);
            return resp->ok();
        }
        /// @brief 删除指定交换机
        /// @param name 交换机名称
        void deleteExchange(const std::string &name)
        {
            deleteExchangeRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_exchange_name(name);
            // 发送请求
            _codec->send(_conn, req);
            waitResponse(rid);
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
                          google::protobuf::Map<std::string, std::string> &qargs)
        {
            declareQueueRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_queue_name(qname);
            req.set_exclusive(qexclusive);
            req.set_durable(qdurable);
            req.set_auto_delete(qauto_delete);
            req.mutable_args()->swap(qargs);
            _codec->send(_conn, req);
            basicResponsePtr resp = waitResponse(rid);
            return resp->ok();
        }
        /// @brief 删除消息队列
        /// @param name 消息队列名称
        void deleteQueue(const std::string &qname)
        {
            deleteQueueRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_queue_name(qname);
            _codec->send(_conn, req);
            waitResponse(rid);
        }
        /// @brief 添加绑定信息
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        /// @param key 绑定关键字
        /// @return 添加成功则返回true 失败返回false
        /// @note 当交换机和消息队列的持久化标志都为true时 绑定信息持久化标志为true才有意义
        bool queueBind(const std::string &ename, const std::string &qname, const std::string &key)
        {
            queueBindRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_queue_name(qname);
            req.set_exchange_name(ename);
            req.set_binding_key(key);
            _codec->send(_conn, req);
            basicResponsePtr resp = waitResponse(rid);
            return resp->ok();
        }
        /// @brief 解除绑定信息
        /// @param ename 交换机名称
        /// @param qname 消息队列名称
        void queueUnBind(const std::string &ename, const std::string &qname)
        {
            queueUnBindRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_queue_name(qname);
            req.set_exchange_name(ename);
            _codec->send(_conn, req);
            waitResponse(rid);
        }
        /// @brief 向指定交换机发布消息
        /// @param ename 交换机名称
        /// @param bp 消息属性
        /// @param body 消息主体
        void basicPublish(const std::string &ename, const BasicProperties *bp, const std::string &body)
        {
            basicPublishRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_exchange_name(ename);
            req.set_body(body);
            if (bp != nullptr)
            {
                req.mutable_properties()->set_id(bp->id());
                req.mutable_properties()->set_delivery_mode(bp->delivery_mode());
                req.mutable_properties()->set_routing_key(bp->routing_key());
            }
            _codec->send(_conn, req);
            waitResponse(rid);
        }
        /// @brief 应答消息
        /// @param qname 消息队列名称
        /// @param msg_id 消息id
        void basicAck(const std::string &qname, const std::string &msg_id)
        {
            basicAckRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_queue_name(qname);
            req.set_msg_id(msg_id);
            _codec->send(_conn, req);
            waitResponse(rid);
        }
        /// @brief 订阅消息
        /// @param tag 消费者标识
        /// @param qname 队列名称
        /// @param auto_ack 自动应答标志
        /// @param cb 消费者回调函数
        /// @return 成功返回true 失败返回false
        bool basicConsume(const std::string &tag, const std::string &qname, bool auto_ack, ConsumerCallback &cb)
        {
            if (_consumer.get() != nullptr)
            {
                warn(logger, "当前信道已订阅其他消息!");
                return false;
            }
            basicConsumeRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_queue_name(qname);
            req.set_consumer_tag(tag);
            req.set_auto_ack(auto_ack);
            _codec->send(_conn, req);
            basicResponsePtr resp = waitResponse(rid);
            if (resp->ok() == false)
            {
                error(logger, "添加订阅失败!");
                return false;
            }
            _consumer = std::make_shared<Consumer>(tag, qname, auto_ack, cb);
            return true;
        }
        /// @brief 取消订阅
        void basicCancel()
        {
            if (_consumer.get() != nullptr)
                return;
            basicCancelRequest req;
            std::string rid = UUIDHelper::uuid();
            req.set_rid(rid);
            req.set_cid(_cid);
            req.set_queue_name(_consumer->qname);
            req.set_consumer_tag(_consumer->tag);
            _codec->send(_conn, req);
            waitResponse(rid);
            _consumer.reset();
        }

        /// @brief 连接收到基础响应后向映射表添加
        /// @param resp 响应句柄
        void putBasicResponse(const basicResponsePtr &resp)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _basic_resp.insert(std::make_pair(resp->rid(), resp));
            _cv.notify_all();
        }
        /// @brief 连接收到推送消息 找到对应的消费者对象 通过回调函数进行消息处理
        /// @param resp 消费响应句柄
        void consume(const basicConsumeResponsePtr &resp)
        {
            if (_consumer.get() == nullptr)
            {
                warn(logger, "消息处理时未找到订阅者信息!");
                return;
            }
            if (_consumer->tag != resp->consumer_tag())
            {
                error(logger, "推送消息中消费者标识与信道消费者标识不一致!");
                return;
            }
            _consumer->callback(resp->consumer_tag(), resp->mutable_properties(), resp->body());
        }

        std::string cid()
        {
            return _cid;
        }

    private:
        /// @brief 等待处理响应
        /// @param rid 响应id
        /// @return 响应句柄
        basicResponsePtr waitResponse(const std::string &rid)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock, [&rid, this]
                     { return _basic_resp.find(rid) != _basic_resp.end(); });
            basicResponsePtr resp = _basic_resp[rid];
            _basic_resp.erase(rid);
            return resp;
        }

    private:
        std::string _cid;                                              ///< 信道id
        muduo::net::TcpConnectionPtr _conn;                            ///< 连接
        ProtobufCodecPtr _codec;                                       ///< 协议处理句柄
        Consumer::ptr _consumer;                                       ///< 信道角色描述
        std::mutex _mutex;                                             ///< 互斥锁
        std::condition_variable _cv;                                   ///< 条件变量
        std::unordered_map<std::string, basicResponsePtr> _basic_resp; ///< 基础响应映射表
    };
    /// @class ChannelManager
    /// @brief 信道管理类
    class ChannelManager
    {
    public:
        using ptr = std::shared_ptr<ChannelManager>; ///< 信道管理句柄
        /// @brief 构造函数
        ChannelManager() {}
        /// @brief 新建一个信道
        /// @param conn muduo连接句柄
        /// @param codec 协议解析句柄
        /// @return 信道句柄
        Channel::ptr create(const muduo::net::TcpConnectionPtr &conn, const ProtobufCodecPtr &codec)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto channel = std::make_shared<Channel>(conn, codec);
            _channels.insert(std::make_pair(channel->cid(), channel));
            return channel;
        }
        /// @brief 移除一个信道
        /// @param cid 信道id
        void remove(const std::string &cid)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _channels.erase(cid);
        }
        /// @brief 获取一个信道
        /// @param cid 信道id
        /// @return 信道句柄
        Channel::ptr get(const std::string &cid)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _channels.find(cid);
            if (it == _channels.end())
                return Channel::ptr();
            return it->second;
        }

    private:
        std::mutex _mutex;                                       ///< 互斥锁
        std::unordered_map<std::string, Channel::ptr> _channels; ///< 信道id和信道句柄的映射表
    };
}