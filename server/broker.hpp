/**
 * @file broker.hpp
 * @brief XuMQ消息队列服务器类的声明文件。
 *
 * 该文件定义了XuMQ消息队列服务器类的实现，负责处理客户端的连接与消息请求，维护虚拟主机、消费者管理和连接管理等。
 *
 * 包含的主要功能：
 * - 处理与客户端的网络连接
 * - 基于 Protobuf 的消息解析和分发
 * - 管理队列、交换机及消费者
 *
 */
#pragma once
#include "muduo/protobuf/codec.h"
#include "muduo/protobuf/dispatcher.h"
#include "muduo/base/Logging.h"
#include "muduo/base/Mutex.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

#include "connection.hpp"
#include "consumer.hpp"
#include "host.hpp"
#include "../common/threadpool.hpp"
#include "../common/msg.pb.h"
#include "../common/protocol.pb.h"
#include "../common/logger.hpp"

namespace XuMQ
{
    const char *DBFILE = "/meta.db";      ///< 数据库名称
    const char *HOSTNAME = "VirtualHost"; ///< 虚拟机名称

    /// @class Server
    /// @brief 服务器类，负责处理客户端请求、管理连接、分发消息。
    class Server
    {
    public:
        using MessagePtr = std::shared_ptr<google::protobuf::Message>; ///< protobuf消息的智能指针类型定义

        /// @brief Server类的构造函数
        /// @param port 服务器监听的端口号
        /// @param basedir 基础目录，用于存储元数据等文件
        Server(int port, const std::string &basedir) : _server(&_baseloop, muduo::net::InetAddress("0.0.0.0", port),
                                                               "Server", muduo::net::TcpServer::kReusePort),
                                                       _dispatcher(std::bind(&Server::onUnknowMessage, this,
                                                                             std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
                                                       _codec(std::make_shared<ProtobufCodec>(std::bind(&ProtobufDispatcher::onProtobufMessage, &_dispatcher,
                                                                                                        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))),
                                                       _virtual_host(std::make_shared<VirtualHost>(HOSTNAME, basedir, basedir + DBFILE)),
                                                       _consumer_manager(std::make_shared<ConsumerManager>()),
                                                       _connection_manager(std::make_shared<ConnectionManager>()),
                                                       _threadpool(std::make_shared<threadpool>())
        {
            debug(logger, "这里虚拟机已经初始化完毕了 开始初始化虚拟机里的消费者了");
            // 初始化队列的消费者管理结构
            QueueMap qm = _virtual_host->allQueues();
            for (auto &q : qm)
                _consumer_manager->initQueueConsumer(q.first);
            // 注册业务请求处理函数
            _dispatcher.registerMessageCallback<XuMQ::openChannelRequest>(std::bind(&Server::onOpenChannel, this, std::placeholders::_1,
                                                                                    std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::closeChannelRequest>(std::bind(&Server::onCloseChannel, this, std::placeholders::_1,
                                                                                     std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::declareExchangeRequest>(std::bind(&Server::onDeclareExchange, this, std::placeholders::_1,
                                                                                        std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::deleteExchangeRequest>(std::bind(&Server::onDeleteExchange, this, std::placeholders::_1,
                                                                                       std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::declareQueueRequest>(std::bind(&Server::onDeClareQueue, this, std::placeholders::_1,
                                                                                     std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::deleteQueueRequest>(std::bind(&Server::onDeleteQueue, this, std::placeholders::_1,
                                                                                    std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::queueBindRequest>(std::bind(&Server::onQueueBind, this, std::placeholders::_1,
                                                                                  std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::queueUnBindRequest>(std::bind(&Server::onQueueUnBind, this, std::placeholders::_1,
                                                                                    std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::basicPublishRequest>(std::bind(&Server::onBasicPublish, this, std::placeholders::_1,
                                                                                     std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::basicAckRequest>(std::bind(&Server::onBasicAck, this, std::placeholders::_1,
                                                                                 std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::basicConsumeRequest>(std::bind(&Server::onBasicConsume, this, std::placeholders::_1,
                                                                                     std::placeholders::_2, std::placeholders::_3));
            _dispatcher.registerMessageCallback<XuMQ::basicCancelRequest>(std::bind(&Server::onBasicCancel, this, std::placeholders::_1,
                                                                                    std::placeholders::_2, std::placeholders::_3));
            _server.setMessageCallback(std::bind(&ProtobufCodec::onMessage, _codec.get(), std::placeholders::_1,
                                                 std::placeholders::_2, std::placeholders::_3));
            _server.setConnectionCallback(std::bind(&Server::onConnection, this, std::placeholders::_1));
        }
        /// @brief 启动服务器，开始监听并处理客户端请求
        void start()
        {
            debug(logger, "启动服务器，开始监听并处理客户端请求");
            _server.start();
            _baseloop.loop();
        }

    private:
        /**
         * @brief 处理打开信道的请求
         * @param conn 客户端连接
         * @param message 打开信道请求消息
         * @param timestamp 消息时间戳
         * @return 操作是否成功
         */
        bool onOpenChannel(const muduo::net::TcpConnectionPtr &conn, const openChannelRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "打开信道时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return false;
            }
            debug(logger, "处理打开信道的请求");
            return mconn->openChannel(message);
        }
        /**
         * @brief 处理关闭信道的请求
         * @param conn 客户端连接
         * @param message 关闭信道请求消息
         * @param timestamp 消息时间戳
         */
        void onCloseChannel(const muduo::net::TcpConnectionPtr &conn, const closeChannelRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "关闭信道时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            debug(logger, "处理关闭信道的请求");
            return mconn->closeChannel(message);
        }
        /**
         * @brief 处理声明交换机的请求
         * @param conn 客户端连接
         * @param message 声明交换机请求消息
         * @param timestamp 消息时间戳
         */
        void onDeclareExchange(const muduo::net::TcpConnectionPtr &conn, const declareExchangeRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "声明交换机时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "声明交换机时 没有找到信道!");
                return;
            }
            debug(logger, "处理声明交换机的请求");
            return cp->declareExchange(message);
        }
        /**
         * @brief 处理删除交换机的请求
         * @param conn 客户端连接
         * @param message 删除交换机请求消息
         * @param timestamp 消息时间戳
         */
        void onDeleteExchange(const muduo::net::TcpConnectionPtr &conn, const deleteExchangeRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "删除交换机时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "删除交换机时 没有找到信道!");
                return;
            }
            debug(logger, "处理删除交换机的请求");
            return cp->deleteExchange(message);
        }
        /**
         * @brief 处理声明队列的请求
         * @param conn 客户端连接
         * @param message 声明队列请求消息
         * @param timestamp 消息时间戳
         */
        void onDeClareQueue(const muduo::net::TcpConnectionPtr &conn, const declareQueueRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "声明队列时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "声明队列时 没有找到信道!");
                return;
            }
            debug(logger, "处理声明队列的请求");
            return cp->declareQueue(message);
        }
        /**
         * @brief 处理删除队列的请求
         * @param conn 客户端连接
         * @param message 删除队列请求消息
         * @param timestamp 消息时间戳
         */
        void onDeleteQueue(const muduo::net::TcpConnectionPtr &conn, const deleteQueueRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "删除队列时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "删除队列时 没有找到信道!");
                return;
            }
            return cp->deleteQueue(message);
        }
        /**
         * @brief 处理队列绑定的请求
         * @param conn 客户端连接
         * @param message 队列绑定请求消息
         * @param timestamp 消息时间戳
         */
        void onQueueBind(const muduo::net::TcpConnectionPtr &conn, const queueBindRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "队列绑定时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "队列绑定时 没有找到信道!");
                return;
            }
            debug(logger, "处理队列绑定的请求");
            return cp->queueBind(message);
        }
        /**
         * @brief 处理队列解绑的请求
         * @param conn 客户端连接
         * @param message 队列解绑请求消息
         * @param timestamp 消息时间戳
         */
        void onQueueUnBind(const muduo::net::TcpConnectionPtr &conn, const queueUnBindRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "队列解绑时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "队列解绑时 没有找到信道!");
                return;
            }
            return cp->queueUnBind(message);
        }
        /**
         * @brief 处理消息发布的请求
         * @param conn 客户端连接
         * @param message 消息发布请求消息
         * @param timestamp 消息时间戳
         */
        void onBasicPublish(const muduo::net::TcpConnectionPtr &conn, const basicPublishRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "消息发布时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "消息发布时 没有找到信道!");
                return;
            }
            return cp->basicPublish(message);
        }
        /**
         * @brief 处理消息应答的请求
         * @param conn 客户端连接
         * @param message 消息应答请求消息
         * @param timestamp 消息时间戳
         */
        void onBasicAck(const muduo::net::TcpConnectionPtr &conn, const basicAckRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "消息确认时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "消息确认时 没有找到信道!");
                return;
            }
            return cp->basicAck(message);
        }
        /**
         * @brief 处理消息订阅的请求
         * @param conn 客户端连接
         * @param message 消息订阅请求消息
         * @param timestamp 消息时间戳
         */
        void onBasicConsume(const muduo::net::TcpConnectionPtr &conn, const basicConsumeRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "消息订阅时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "消息订阅时 没有找到信道!");
                return;
            }
            return cp->basicConsume(message);
        }
        /**
         * @brief 处理取消订阅的请求
         * @param conn 客户端连接
         * @param message 取消订阅请求消息
         * @param timestamp 消息时间戳
         */
        void onBasicCancel(const muduo::net::TcpConnectionPtr &conn, const basicCancelRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "取消订阅时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            Channel::ptr cp = mconn->getChannel(message->cid());
            if (cp.get() == nullptr)
            {
                error(logger, "取消订阅时 没有找到信道!");
                return;
            }
            return cp->basicCancel(message);
        }
        /**
         * @brief 处理未知消息类型的请求
         * @param conn 客户端连接
         * @param message 未知类型的消息
         * @param timestamp 消息时间戳
         */
        void onUnknowMessage(const muduo::net::TcpConnectionPtr &conn, const Server::MessagePtr message, muduo::Timestamp)
        {
            INFO("onUnknowMessage: %s", message->GetTypeName());
            conn->shutdown();
        }
        /**
         * @brief 处理新连接的回调函数
         * @param conn 新建立的TCP连接
         */
        void onConnection(const muduo::net::TcpConnectionPtr &conn)
        {
            if (conn->connected())
            {
                debug(logger, "处理新连接的回调函数 这里就需要传入已经处理好的虚拟机了");
                _connection_manager->newConnection(_virtual_host, _consumer_manager, _codec, conn, _threadpool);
            }
            else
            {
                _connection_manager->deleteConnection(conn);
            }
        }

    private:
        muduo::net::EventLoop _baseloop;            ///< 基础时间循环
        muduo::net::TcpServer _server;              ///< 服务器对象
        ProtobufDispatcher _dispatcher;             ///< 请求分发器对象 -> 注册请求处理函数
        ProtobufCodecPtr _codec;                    ///< protobuf协议处理器 -> 对收到的请求数据进行protobuf协议处理
        VirtualHost::ptr _virtual_host;             ///< 虚拟机句柄
        ConsumerManager::ptr _consumer_manager;     ///< 消费者管理句柄
        ConnectionManager::ptr _connection_manager; ///< 连接管理句柄
        threadpool::ptr _threadpool;                ///< 线程池管理句柄
    };
}