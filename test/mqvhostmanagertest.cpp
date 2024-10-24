#include "../server/host.hpp"
#include <gtest/gtest.h>

using none_map = google::protobuf::Map<std::string, std::string>;

class HostTest : public testing::Test
{
public:
    virtual void SetUp() override
    {
        _vhmp = std::make_shared<XuMQ::VirtualHostManager>();
        _vhmp->declareVirtualHost("host1", "./data/host1/message", "./data/host1/host1.db");
        _host = _vhmp->selectVirtualHost("host1");
        std::cout << _vhmp->exists("host1") << " " << _vhmp->exists("host2") << " " << _vhmp->size() << "\n";
        none_map map = none_map();
        _host = std::make_shared<XuMQ::VirtualHost>("host1", "./data/host1/message", "./data/host1/host1.db");
        _host->declareExchange("exchange1", XuMQ::ExchangeType::DIRECT, true, false, map);
        _host->declareExchange("exchange2", XuMQ::ExchangeType::DIRECT, true, false, map);
        _host->declareExchange("exchange3", XuMQ::ExchangeType::DIRECT, true, false, map);

        _host->declareQueue("queue1", true, false, false, map);
        _host->declareQueue("queue2", true, false, false, map);
        _host->declareQueue("queue3", true, false, false, map);

        _host->bind("exchange1", "queue1", "news.music.#");
        _host->bind("exchange1", "queue2", "news.music.#");
        _host->bind("exchange1", "queue3", "news.music.#");

        _host->bind("exchange2", "queue2", "news.music.#");
        _host->bind("exchange2", "queue3", "news.music.#");

        _host->bind("exchange3", "queue3", "news.music.#");

        _host->basicPublish("queue1", nullptr, "hello world 1");
        _host->basicPublish("queue1", nullptr, "hello world 2");
        _host->basicPublish("queue1", nullptr, "hello world 3");
        _host->basicPublish("queue2", nullptr, "hello world 1");
        _host->basicPublish("queue2", nullptr, "hello world 2");
        _host->basicPublish("queue3", nullptr, "hello world 1");
    }
    virtual void TearDown() override
    {
        _host->clear();
        _vhmp->clear();
    }

public:
    XuMQ::VirtualHost::ptr _host;
    XuMQ::VirtualHostManager::ptr _vhmp;
};

TEST_F(HostTest, init_test)
{
    ASSERT_TRUE(_host->existsExchange("exchange1"));
    ASSERT_TRUE(_host->existsExchange("exchange2"));
    ASSERT_TRUE(_host->existsExchange("exchange3"));
    ASSERT_TRUE(_host->existsQueue("queue1"));
    ASSERT_TRUE(_host->existsQueue("queue2"));
    ASSERT_TRUE(_host->existsQueue("queue3"));

    ASSERT_TRUE(_host->existsBinding("exchange1", "queue1"));
    ASSERT_TRUE(_host->existsBinding("exchange1", "queue2"));
    ASSERT_TRUE(_host->existsBinding("exchange1", "queue3"));
    ASSERT_TRUE(_host->existsBinding("exchange2", "queue2"));
    ASSERT_TRUE(_host->existsBinding("exchange2", "queue3"));
    ASSERT_TRUE(_host->existsBinding("exchange3", "queue3"));

    XuMQ::MessagePtr msg1 = _host->basicConsume("queue1");
    ASSERT_EQ(msg1->payload().body(), std::string("hello world 1"));
    XuMQ::MessagePtr msg2 = _host->basicConsume("queue1");
    ASSERT_EQ(msg2->payload().body(), std::string("hello world 2"));
    XuMQ::MessagePtr msg3 = _host->basicConsume("queue1");
    ASSERT_EQ(msg3->payload().body(), std::string("hello world 3"));
    XuMQ::MessagePtr msg4 = _host->basicConsume("queue1");
    ASSERT_EQ(msg4.get(), nullptr);
}

TEST_F(HostTest, delete_exchange)
{
    _host->deleteExchange("exchange1");
    ASSERT_FALSE(_host->existsBinding("exchange1", "queue1"));
    ASSERT_FALSE(_host->existsBinding("exchange1", "queue2"));
    ASSERT_FALSE(_host->existsBinding("exchange1", "queue3"));
}

TEST_F(HostTest, delete_queue)
{
    _host->deleteQueue("queue3");
    ASSERT_FALSE(_host->existsBinding("exchange1", "queue3"));
    ASSERT_FALSE(_host->existsBinding("exchange2", "queue3"));
    ASSERT_FALSE(_host->existsBinding("exchange3", "queue3"));
    XuMQ::MessagePtr msg1 = _host->basicConsume("queue3");
    ASSERT_EQ(msg1.get(), nullptr);
}

TEST_F(HostTest, ack_msg)
{
    XuMQ::MessagePtr msg1 = _host->basicConsume("queue2");
    ASSERT_NE(msg1.get(), nullptr);
    ASSERT_EQ(msg1->payload().body(), std::string("hello world 1"));
    _host->basicAck("queue2", msg1->payload().properties().id());
    XuMQ::MessagePtr msg2 = _host->basicConsume("queue2");
    ASSERT_NE(msg2.get(), nullptr);
    _host->basicAck("queue2", msg2->payload().properties().id());
    ASSERT_EQ(msg2->payload().body(), std::string("hello world 2"));
    XuMQ::MessagePtr msg3 = _host->basicConsume("queue2");
    ASSERT_EQ(msg3.get(), nullptr);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    int res = RUN_ALL_TESTS();
    return 0;
}