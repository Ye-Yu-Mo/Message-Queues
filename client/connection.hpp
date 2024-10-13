#pragma once

#include "muduo/protobuf/dispatcher.h"
#include "muduo/protobuf/codec.h"

#include "muduo/base/Mutex.h"
#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpClient.h"
#include "muduo/net/EventLoopThread.h"
#include "muduo/base/CountDownLatch.h"
#include "channel.hpp"
#include "worker.hpp"
#include "../common/logger.hpp"

namespace XuMQ
{
    /// @class Connection
    /// @brief 连接管理模块
    class Connection
    {
    public:
        /// @brief 连接构造函数
        /// @param sip 服务器ip
        /// @param sport 服务器端口号
        /// @param worker 异步工作器句柄
        Connection(const std::string &sip, int sport, const AsyncWorker::ptr &worker)
            : _latch(1), _client(worker->_loopthread.startLoop(), muduo::net::InetAddress(sip, sport), "Client"),
              _dispatcher(std::bind(&Connection::onUnknowMessage, this,
                                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
              _codec(std::make_shared<ProtobufCodec>(std::bind(&ProtobufDispatcher::onProtobufMessage, &_dispatcher,
                                                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))),
              _worker(worker), _channels(std::make_shared<ChannelManager>())
        {
            _dispatcher.registerMessageCallback<basicResponse>(std::bind(&Connection::commonResponse, this, std::placeholders::_1,
                                                                         std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<basicConsumeResponse>(std::bind(&Connection::consumeResponse, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3));
            _client.setMessageCallback(std::bind(&ProtobufCodec::onMessage, _codec, std::placeholders::_1,
                                                 std::placeholders::_2, std::placeholders::_3));
            _client.setConnectionCallback(std::bind(&Connection::onConnection, this, std::placeholders::_1));
            _client.connect();
            _latch.wait();
        }
        /// @brief 创建信道
        /// @return 信道句柄
        Channel::ptr openChannel()
        {
            Channel::ptr channel = _channels->create(_conn, _codec);
            bool ret = channel->openChannel();
            if (ret == false)
            {
                error(logger, "打开信道失败!");
                return Channel::ptr();
            }
            return channel;
        }
        /// @brief 关闭信道
        /// @param channel 信道句柄
        void closeChannel(const Channel::ptr &channel)
        {
            channel->closeChannel();
            _channels->remove(channel->cid());
        }

    private:
        /// @brief 处理一般响应的回调函数
        /// @param conn muduo连接
        /// @param message 响应句柄
        /// @param  时间戳
        void commonResponse(const muduo::net::TcpConnectionPtr &conn, const basicResponsePtr message, muduo::Timestamp)
        {
            // 找到信道
            Channel::ptr channel = _channels->get(message->cid());
            if (channel == nullptr)
            {
                error(logger, "未找到信道!");
                return;
            }
            // 将得到的响应对象 添加到信道的基础响应中
            channel->putBasicResponse(message);
        }
        /// @brief 处理消费响应的回调函数
        /// @param conn muduo连接
        /// @param message 消费响应
        /// @param  时间戳
        void consumeResponse(const muduo::net::TcpConnectionPtr &conn, const basicConsumeResponsePtr message, muduo::Timestamp)
        {
            // 找到信道
            Channel::ptr channel = _channels->get(message->cid());
            if (channel == nullptr)
            {
                error(logger, "未找到信道!");
                return;
            }
            // 封装异步任务交给线程池
            _worker->_threadpool.push([channel, message]()
                                      { channel->consume(message); });
        }
        /// @brief 连接回调函数
        /// @param conn muduo连接
        void onConnection(const muduo::net::TcpConnectionPtr &conn)
        {
            if (conn->connected())
            {
                _latch.countDown();
                _conn = conn;
            }
            else
            {
                _conn.reset();
            }
        }
        /// @brief 未知响应回调函数
        /// @param conn muduo连接
        /// @param message 未知响应
        /// @param  时间戳
        void onUnknowMessage(const muduo::net::TcpConnectionPtr &conn, const MessagePtr &message, muduo::Timestamp)
        {
            info(logger, "onUnknowMessage: %s", message->GetTypeName());
            conn->shutdown();
        }

    private:
        muduo::CountDownLatch _latch;       ///< 实现同步
        muduo::net::TcpConnectionPtr _conn; ///< 客户端连接
        muduo::net::TcpClient _client;      ///< 客户端句柄
        ProtobufDispatcher _dispatcher;     ///< 请求分发器
        ProtobufCodecPtr _codec;            ///< 协议处理器
        AsyncWorker::ptr _worker;           ///< 异步线程工作器
        ChannelManager::ptr _channels;      ///< 信道管理句柄
    };

}