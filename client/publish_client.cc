#include "connection.hpp"
int main()
{
    // 实例化异步工作线程对象
    XuMQ::AsyncWorker::ptr awp = std::make_shared<XuMQ::AsyncWorker>();
    // 实例化连接对象
    XuMQ::Connection::ptr conn = std::make_shared<XuMQ::Connection>("127.0.0.1", 8888, awp);
    // 通过连接创建信道
    XuMQ::Channel::ptr channel = conn->openChannel();
    // 完成服务
    // 声明交换机exchange1 
    google::protobuf::Map<std::string, std::string> tmp;
    channel->declareExchange("exchange1", XuMQ::ExchangeType::TOPIC, true, false, tmp);
    // 声明队列queue1
    channel->declareQueue("queue1", true, false, false, tmp);
    // 声明队列queue2
    channel->declareQueue("queue2", true, false, false, tmp);
    // 绑定queue1-exchange1 设置binding_key = queue1
    channel->queueBind("exchange1", "queue1", "queue1");
    // 绑定queue2-exchange1 设置binding_key = news.music.#
    channel->queueBind("exchange1", "queue2", "news.music.#");
    // 发布消息

    for (int i = 0; i < 10; i++)
    {
        XuMQ::BasicProperties bp;
        bp.set_id(XuMQ::UUIDHelper::uuid());
        bp.set_delivery_mode(XuMQ::DeliveryMode::DURABLE);
        bp.set_routing_key("news.music.pop");
        channel->basicPublish("exchange1", &bp, "hello-" + std::to_string(i));
    }
    XuMQ::BasicProperties bp1;
    bp1.set_id(XuMQ::UUIDHelper::uuid());
    bp1.set_delivery_mode(XuMQ::DeliveryMode::DURABLE);
    bp1.set_routing_key("news.music.sport");
    channel->basicPublish("exchange1", &bp1, "hello XU");
    XuMQ::BasicProperties bp2;
    bp2.set_id(XuMQ::UUIDHelper::uuid());
    bp2.set_delivery_mode(XuMQ::DeliveryMode::DURABLE);
    bp2.set_routing_key("news.sport");
    channel->basicPublish("exchange1", &bp2, "hello ???");
    // 关闭信道
    conn->closeChannel(channel);
    return 0;
}