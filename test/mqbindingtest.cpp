#include "../server/binding.hpp"
#include <gtest/gtest.h>

XuMQ::BindingManager::ptr bmp;

class BindingTest : public testing::Environment
{
public:
    virtual void SetUp() override
    {
        bmp = std::make_shared<XuMQ::BindingManager>("./data/meta.db");
    }
    virtual void TearDown() override
    {
        bmp->clear();
        info(XuMQ::logger, "清理完成");
    }
};

TEST(bind_test, recovery_test)
{
    ASSERT_TRUE(bmp->exists("exchange2", "queue3"));
}

// TEST(bind_test, insert_test)
// {
//     bmp->bind("exchange1", "queue1", "news.music.#", true);
//     bmp->bind("exchange1", "queue2", "news.sport.#", true);
//     bmp->bind("exchange1", "queue3", "news.sport.#", true);
//     bmp->bind("exchange2", "queue1", "news.music.pop", true);
//     bmp->bind("exchange2", "queue2", "news.spot.pingpang", true);
//     bmp->bind("exchange2", "queue3", "news.sport.#", true);

//     // ASSERT_EQ(bmp->size(), 5);
// }

// TEST(bind_test, select_test)
// {
//     ASSERT_TRUE(bmp->exists("exchange1", "queue1"));
//     ASSERT_TRUE(bmp->exists("exchange1", "queue2"));
//     ASSERT_TRUE(bmp->exists("exchange1", "queue3"));
//     ASSERT_TRUE(bmp->exists("exchange2", "queue2"));
//     ASSERT_TRUE(bmp->exists("exchange2", "queue1"));

//     XuMQ::Binding::ptr bp = bmp->getBinding("exchange1", "queue1");
//     ASSERT_NE(bp.get(), nullptr);
//     ASSERT_EQ(bp->exchange_name, std::string("exchange1"));
//     ASSERT_EQ(bp->msgqueue_name, std::string("queue1"));
//     ASSERT_EQ(bp->binding_key, std::string("news.music.#"));
// }

// TEST(bind_test, exchange_test)
// {
//     XuMQ::MsgQueueBindingMap mqbm = bmp->getExchangeBindings("exchange2");
//     ASSERT_NE(mqbm.find("queue1"), mqbm.end());
//     ASSERT_NE(mqbm.find("queue2"), mqbm.end());
// }

// TEST(bind_test, remove_queue_test)
// {
//     bmp->removeMsgQueueBindings("queue1");
//     ASSERT_FALSE(bmp->exists("exchange1", "queue1"));
//     ASSERT_TRUE(bmp->exists("exchange1", "queue2"));
//     ASSERT_TRUE(bmp->exists("exchange1", "queue3"));
//     ASSERT_TRUE(bmp->exists("exchange2", "queue2"));
//     ASSERT_FALSE(bmp->exists("exchange2", "queue1"));
// }

// TEST(bind_test, remove_exchange_test)
// {
//     bmp->removeExchangeBindings("exchange1");
//     ASSERT_FALSE(bmp->exists("exchange1", "queue1"));
//     ASSERT_FALSE(bmp->exists("exchange1", "queue2"));
//     ASSERT_FALSE(bmp->exists("exchange1", "queue3"));
//     ASSERT_TRUE(bmp->exists("exchange2", "queue2"));
//     ASSERT_FALSE(bmp->exists("exchange2", "queue1"));
// }

// TEST(bind_test, remove_single_test)
// {
//     ASSERT_TRUE(bmp->exists("exchange2", "queue2"));
//     bmp->unbind("exchange2", "queue2");
//     ASSERT_FALSE(bmp->exists("exchange2", "queue2"));
// }

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new BindingTest);
    int res = RUN_ALL_TESTS();
    return 0;
}