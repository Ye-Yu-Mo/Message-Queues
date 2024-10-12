#include "../server/connection.hpp"

int main()
{
    XuMQ::ConnectionManager::ptr cmp = std::make_shared<XuMQ::ConnectionManager>();
    cmp->newConnection(std::make_shared<XuMQ::VirtualHost>("host1", "./data/host1/message/", "./data/host1/host1.db"),
                       std::make_shared<XuMQ::ConsumerManager>(),
                       XuMQ::ProtobufCodecPtr(),
                       muduo::net::TcpConnectionPtr(),
                       XuMQ::threadpool::ptr());
    
    return 0;
}