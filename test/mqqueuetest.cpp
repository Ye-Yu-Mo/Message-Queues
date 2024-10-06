#include "../server/queue.hpp"
#include <gtest/gtest.h>

XuMQ::MsgQueueManager::ptr mqp;

class QueueTest : public testing::Environment
{
public:
    virtual void SetUp() override
    {
        mqp = std::make_shared<XuMQ::MsgQueueManager>("./data/meta.db");
    }
    virtual void TearDown() override
    {
        mqp->clear();
        info(XuMQ::logger, "清理完成");
    }
};

TEST(queue_test, insert_test)
{
    std::unordered_map<std::string, std::string> map = {{"k1", "v1"}};
    mqp->declareQueue("queue1", true, false, false, map);
    mqp->declareQueue("queue2", true, false, false, map);
    mqp->declareQueue("queue3", true, false, false, map);
    mqp->declareQueue("queue4", true, false, false, map);
    ASSERT_EQ(mqp->size(), 4);
}

TEST(queue_test, select_test)
{
    ASSERT_TRUE(mqp->exists("queue1"));
    ASSERT_TRUE(mqp->exists("queue2"));
    ASSERT_TRUE(mqp->exists("queue3"));
    ASSERT_TRUE(mqp->exists("queue4"));

    XuMQ::MsgQueue::ptr mq = mqp->selectQueue("queue3");
    ASSERT_NE(mq.get(), nullptr);
    ASSERT_EQ(mq->name, "queue3");
    ASSERT_TRUE(mq->durable);
    ASSERT_FALSE(mq->auto_delete);
    ASSERT_FALSE(mq->exclusive);
    ASSERT_EQ(mq->getArgs(), std::string("k1=v1"));
}
TEST(queue_test, remove_test)
{
    mqp->deleteQueue("queue4");
    XuMQ::MsgQueue::ptr mq = mqp->selectQueue("queue4");
    ASSERT_EQ(mq.get(), nullptr);
    ASSERT_FALSE(mqp->exists("queue4"));
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new QueueTest);
    int res = RUN_ALL_TESTS();
    return 0;
}