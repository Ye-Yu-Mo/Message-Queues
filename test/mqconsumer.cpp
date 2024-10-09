#include "../server/consumer.hpp"
#include <gtest/gtest.h>

XuMQ::ConsumerManager::ptr cmp;

class ConsumerTest : public testing::Environment
{
public:
    virtual void SetUp() override
    {
        cmp = std::make_shared<XuMQ::ConsumerManager>();
    }
    virtual void TearDown() override
    {
        cmp->clear();
        info(XuMQ::logger, "清理完成");
    }
};

void CallBack(const std::string &tag, const XuMQ::BasicProperties *bp, const std::string &body)
{
    DEBUG("tag %s 消费了消息: %s", tag.c_str(), body.c_str());
}

TEST(consumer_test, insert_test)
{
    cmp->initQueueConsumer("queue1");

    cmp->create("consumer1", "queue1", false, CallBack);
    cmp->create("consumer2", "queue1", false, CallBack);
    cmp->create("consumer3", "queue1", false, CallBack);

    ASSERT_TRUE(cmp->exists("consumer1", "queue1"));
    ASSERT_TRUE(cmp->exists("consumer2", "queue1"));
    ASSERT_TRUE(cmp->exists("consumer3", "queue1"));

}

TEST(consumer_test, remove_test)
{
    cmp->remove("consumer1", "queue1");

    ASSERT_FALSE(cmp->exists("consumer1", "queue1"));
    ASSERT_TRUE(cmp->exists("consumer2", "queue1"));
    ASSERT_TRUE(cmp->exists("consumer3", "queue1"));
}

TEST(consumer_test, choose_test)
{
    XuMQ::Consumer::ptr cp = cmp->choose("queue1");
    ASSERT_NE(cp.get(), nullptr);
    ASSERT_EQ(cp->tag, "consumer2");
    cp = cmp->choose("queue1");
    ASSERT_NE(cp.get(), nullptr);
    ASSERT_EQ(cp->tag, "consumer3");
    cp = cmp->choose("queue1");
    ASSERT_NE(cp.get(), nullptr);
    ASSERT_EQ(cp->tag, "consumer2");
    cp = cmp->choose("queue1");
    ASSERT_NE(cp.get(), nullptr);
    ASSERT_EQ(cp->tag, "consumer3");
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new ConsumerTest);
    int res = RUN_ALL_TESTS();
    return 0;
}