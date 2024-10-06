#include "../server/message.hpp"
#include <gtest/gtest.h>

XuMQ::MessageManager::ptr mmp;

class BindingTest : public testing::Environment
{
public:
    virtual void SetUp() override
    {
        mmp = std::make_shared<XuMQ::MessageManager>("./data/message/");
        mmp->initQueueMessage("queue1");
    }
    virtual void TearDown() override
    {
        // mmp->clear();
        info(XuMQ::logger, "清理完成");
    }
};

// TEST(message_test, insert_test)
// {
//     XuMQ::BasicProperties properties;
//     properties.set_id(XuMQ::UUIDHelper::uuid());
//     XuMQ::DeliveryMode mode = XuMQ::DeliveryMode::DURABLE;
//     properties.set_delivery_mode(mode);
//     properties.set_routing_key("music.pop.#");

//     mmp->insert("queue1", &properties, "hello xu 1", XuMQ::DeliveryMode::DURABLE);
//     mmp->insert("queue1", nullptr, "hello xu 2", XuMQ::DeliveryMode::DURABLE);
//     mmp->insert("queue1", nullptr, "hello xu 3", XuMQ::DeliveryMode::DURABLE);
//     mmp->insert("queue1", nullptr, "hello xu 4", XuMQ::DeliveryMode::DURABLE);
//     mmp->insert("queue1", nullptr, "hello xu 5", XuMQ::DeliveryMode::UNDURABLE);
//     mmp->insert("queue1", nullptr, "hello xu 6", XuMQ::DeliveryMode::DURABLE);
//     mmp->insert("queue1", nullptr, "hello xu 7", XuMQ::DeliveryMode::DURABLE);
//     mmp->insert("queue1", nullptr, "hello xu 8", XuMQ::DeliveryMode::DURABLE);
//     mmp->insert("queue1", nullptr, "hello xu 9", XuMQ::DeliveryMode::DURABLE);
//     mmp->insert("queue1", nullptr, "hello xu 10", XuMQ::DeliveryMode::DURABLE);
//     ASSERT_EQ(mmp->totalCount("queue1"), 9);
//     ASSERT_EQ(mmp->availableCount("queue1"), 10);
//     ASSERT_EQ(mmp->durableCount("queue1"), 9);
//     ASSERT_EQ(mmp->waitAckCount("queue1"), 0);
// }

// TEST(message_test, recovery_test)
// {
//     // ASSERT_EQ(mmp->availableCount("queue1"), 9);
//     // ASSERT_EQ(mmp->availableCount("queue1"), 9);
//     // ASSERT_EQ(mmp->availableCount("queue1"), 9);
//     // ASSERT_EQ(mmp->availableCount("queue1"), 9);

//     XuMQ::MessagePtr msg1 = mmp->front("queue1");
//     ASSERT_NE(msg1, nullptr);

//     debug(XuMQ::logger, "恢复文件内容 %s", msg1->payload().body().c_str());
//     XuMQ::MessagePtr msg2 = mmp->front("queue1");
//     ASSERT_NE(msg2, nullptr);
//     debug(XuMQ::logger, "恢复文件内容 %s", msg2->payload().body().c_str());

//     XuMQ::MessagePtr msg3 = mmp->front("queue1");
//     ASSERT_NE(msg3, nullptr);
//     debug(XuMQ::logger, "恢复文件内容 %s", msg3->payload().body().c_str());

//     XuMQ::MessagePtr msg4 = mmp->front("queue1");
//     ASSERT_NE(msg4, nullptr);
//     debug(XuMQ::logger, "恢复文件内容 %s", msg4->payload().body().c_str());

//     XuMQ::MessagePtr msg5 = mmp->front("queue1");
//     ASSERT_NE(msg5, nullptr);
//     debug(XuMQ::logger, "恢复文件内容 %s", msg5->payload().body().c_str());

//     // ASSERT_EQ(mmp->waitAckCount("queue1"), 5);
// }

TEST(message_test, delete_test)
{
    XuMQ::MessagePtr msg1 = mmp->front("queue1");

    ASSERT_NE(msg1.get(), nullptr);
    ASSERT_EQ(msg1->payload().body(), std::string("hello xu 2"));
    // ASSERT_EQ(mmp->durableCount("queue1"), 9);
    // ASSERT_EQ(mmp->availableCount("queue1"), 9);
    // ASSERT_EQ(mmp->totalCount("queue1"), 9);
    // XuMQ::MessagePtr msg1 = mmp->front("queue1");
    // ASSERT_EQ(mmp->durableCount("queue1"), 9);

    // ASSERT_NE(msg1.get(), nullptr);
    // ASSERT_EQ(msg1->payload().body(), std::string("hello xu 1"));
    // ASSERT_EQ(mmp->availableCount("queue1"), 8);
    // ASSERT_EQ(mmp->waitAckCount("queue1"), 1);
    // mmp->ack("queue1", msg1->payload().properties().id());
    // ASSERT_EQ(mmp->availableCount("queue1"), 8);
    // ASSERT_EQ(mmp->waitAckCount("queue1"), 0);
    // ASSERT_EQ(mmp->durableCount("queue1"), 8);
    // ASSERT_EQ(mmp->totalCount("queue1"), 9);
    
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new BindingTest);
    int res = RUN_ALL_TESTS();
    return 0;
}