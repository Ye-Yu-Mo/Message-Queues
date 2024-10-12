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
    const char *DBFILE = "/meta.db";
    const char *HOSTNAME = "VirtualHost";
    class Server
    {
    public:
        using MessagePtr = std::shared_ptr<google::protobuf::Message>;

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
        void start()
        {
            _server.start();
            _baseloop.loop();
        }

    private:
        bool onOpenChannel(const muduo::net::TcpConnectionPtr &conn, const openChannelRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "打开信道时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return false;
            }
            return mconn->openChannel(message);
        }
        void onCloseChannel(const muduo::net::TcpConnectionPtr &conn, const closeChannelRequestPtr message, muduo::Timestamp)
        {
            Connection::ptr mconn = _connection_manager->getConnection(conn);
            if (mconn.get() == nullptr)
            {
                error(logger, "关闭信道时 没有找到连接对应的Connection对象!");
                conn->shutdown();
                return;
            }
            return mconn->closeChannel(message);
        }
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
            return cp->declareExchange(message);
        }
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
            return cp->deleteExchange(message);
        }
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
            return cp->declareQueue(message);
        }
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
            return cp->queueBind(message);
        }
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

        void onUnknowMessage(const muduo::net::TcpConnectionPtr &conn, const Server::MessagePtr message, muduo::Timestamp)
        {
            INFO("onUnknowMessage: %s", message->GetTypeName());
            conn->shutdown();
        }
        void onConnection(const muduo::net::TcpConnectionPtr &conn)
        {
            if (conn->connected())
            {
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