/**
 * @file connection.hpp
 * @brief 声明了连接类和连接管理类，主要用于管理连接的建立、维护与信道的操作。
 * 
 * 本文件定义了两个主要类：
 * - Connection 类：负责管理具体的连接实例，包含信道的开启与关闭操作。
 * - ConnectionManager 类：负责管理多个连接实例，包括新建、删除和获取连接。
 */


#include "channel.hpp"

namespace XuMQ
{
    /// @class Connection
    /// @brief 连接类
    class Connection
    {
    public:
        using ptr = std::shared_ptr<Connection>; ///< 连接句柄
        /// @brief 连接构造函数
        /// @param host 虚拟机
        /// @param cmp 消费者管理句柄
        /// @param codec 协议处理句柄
        /// @param conn muduo连接管理句柄
        /// @param pool 线程池管理句柄
        Connection(const VirtualHost::ptr &host, const ConsumerManager::ptr &cmp,
                   const ProtobufCodecPtr &codec, const muduo::net::TcpConnectionPtr &conn,
                   const threadpool::ptr &pool) : _conn(conn), _codec(codec), _cmp(cmp), _host(host), _pool(pool),
                                                  _channels(std::make_shared<ChannelManager>()) {}
        /// @brief 打开信道
        /// @param req 打开信道请求
        bool openChannel(const openChannelRequestPtr &req)
        {
            // 判断信道id是否重复 创建信道
            bool ret = _channels->openChannel(req->rid(), _host, _cmp, _codec, _conn, _pool);
            if (ret == false)
            {
                error(logger, "创建信道时 信道id重复!");
                basicRespFunc(false, req->rid(), req->cid());
                return false;
            }
            // 给客户端回复
            basicRespFunc(true, req->rid(), req->cid());
            return true;
        }
        /// @brief 关闭信道
        /// @param req 关闭信道请求
        void closeChannel(const closeChannelRequestPtr &req)
        {
            _channels->closeChannel(req->cid());
            basicRespFunc(true, req->rid(), req->cid());
        }

        Channel::ptr getChannel(const std::string &cid)
        {
            return _channels->getChannel(cid);
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

    private:
        muduo::net::TcpConnectionPtr _conn; ///< muduo连接管理句柄
        ProtobufCodecPtr _codec;            ///< 协议处理句柄
        ConsumerManager::ptr _cmp;          ///< 消费者管理句柄
        VirtualHost::ptr _host;             ///< 虚拟机
        threadpool::ptr _pool;              ///< 线程池
        ChannelManager::ptr _channels;      ///< 信道管理句柄
    };

    /// @class ConnectionManager
    /// @brief 连接管理类
    class ConnectionManager
    {
    public:
        using ptr = std::shared_ptr<ConnectionManager>; ///< 连接管理句柄
        /// @brief 构造函数
        ConnectionManager() {}
        /// @brief 新建一个连接
        /// @param host 虚拟机
        /// @param cmp 消费者管理句柄
        /// @param codec 协议处理句柄
        /// @param conn muduo连接管理句柄
        /// @param pool 线程池管理句柄
        void newConnection(const VirtualHost::ptr &host, const ConsumerManager::ptr &cmp,
                           const ProtobufCodecPtr &codec, const muduo::net::TcpConnectionPtr &conn,
                           const threadpool::ptr &pool)
        {
            std::unique_lock lock(_mutex);
            auto it = _conns.find(conn);
            if (it != _conns.end())
                return;
            Connection::ptr self_conn = std::make_shared<Connection>(host, cmp, codec, conn, pool);
            _conns.insert(std::make_pair(conn, self_conn));
        }
        /// @brief 删除一个连接
        /// @param conn muduo连接管理句柄
        void deleteConnection(const muduo::net::TcpConnectionPtr &conn)
        {
            std::unique_lock lock(_mutex);
            _conns.erase(conn);
        }
        /// @brief 获取一个连接
        /// @param conn muduo连接管理句柄
        /// @return 连接句柄
        Connection::ptr getConnection(const muduo::net::TcpConnectionPtr &conn)
        {
            std::unique_lock lock(_mutex);
            auto it = _conns.find(conn);
            if (it == _conns.end())
                return Connection::ptr();
            return it->second;
        }

    private:
        std::mutex _mutex;                                                        ///< 互斥锁
        std::unordered_map<muduo::net::TcpConnectionPtr, Connection::ptr> _conns; ///< 一个从muduo连接管理句柄到连接管理句柄的映射表
    };
}