#include "connection.hpp"
#include "../common/logger.hpp"

void callBack(XuMQ::Channel::ptr &channel, const std::string &consumer_tag, const XuMQ::BasicProperties *bp, const std::string &body)
{
    info(XuMQ::logger, "消费者%s 消费的消息是：%s", consumer_tag.c_str(), body.c_str());
    channel->basicAck(bp->id());
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        info(XuMQ::logger, "usage: ./consume_client queue1");
    }
    // 实例化异步工作线程对象
    XuMQ::AsyncWorker::ptr awp = std::make_shared<XuMQ::AsyncWorker>();
    // 实例化连接对象
    XuMQ::Connection::ptr conn = std::make_shared<XuMQ::Connection>("127.0.0.1", 8888, awp);
    // 通过连接创建信道
    XuMQ::Channel::ptr channel = conn->openChannel();

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
    // 订阅指定队列消息
    auto func = std::bind(callBack, channel, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    channel->basicConsume("consumer1", argv[1], false, func);
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    conn->closeChannel(channel);
    return 0;
}