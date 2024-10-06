#include "../server/exchange.hpp"
#include <gtest/gtest.h>
XuMQ::ExchangeManager::ptr emp;

class ExchangeTest : public testing::Environment
{
public:
    virtual void SetUp() override
    {
        emp = std::make_shared<XuMQ::ExchangeManager>("./data/meta.db");
        debug(XuMQ::logger, "exchange1:%d", emp->exists("exchange1"));
        debug(XuMQ::logger, "exchange2:%d", emp->exists("exchange2"));
        debug(XuMQ::logger, "exchange3:%d", emp->exists("exchange3"));
        debug(XuMQ::logger, "exchange4:%d", emp->exists("exchange4"));
    }
    virtual void TearDown() override
    {
        emp->clear();
        info(XuMQ::logger, "清理完成");
    }
};

TEST(exchange_test, insert_test)
{
    std::unordered_map<std::string, std::string> map = {{"k1", "v1"}, {"k2", "v2"}};
    emp->declareExchange("exchange1", XuMQ::ExchangeType::DIRECT, true, false, map);
    emp->declareExchange("exchange2", XuMQ::ExchangeType::DIRECT, true, false, map);
    emp->declareExchange("exchange3", XuMQ::ExchangeType::DIRECT, true, false, map);
    emp->declareExchange("exchange4", XuMQ::ExchangeType::DIRECT, true, false, map);
    ASSERT_EQ(emp->size(), 4);
}

TEST(exchange_test, select_test)
{
    // ASSERT_EQ(emp->exists("exchange1"), true);
    // ASSERT_EQ(emp->exists("exchange2"), false);
    // ASSERT_EQ(emp->exists("exchange3"), true);
    // ASSERT_EQ(emp->exists("exchange4"), true);
    debug(XuMQ::logger, "exchange1:%d", emp->exists("exchange1"));
    debug(XuMQ::logger, "exchange2:%d", emp->exists("exchange2"));
    debug(XuMQ::logger, "exchange3:%d", emp->exists("exchange3"));
    debug(XuMQ::logger, "exchange4:%d", emp->exists("exchange4"));

    XuMQ::Exchange::ptr exp = emp->selectExchange("exchange3");
    // ASSERT_NE(exp.get(), nullptr);
    ASSERT_EQ(exp->name, "exchange3");
    ASSERT_EQ(exp->durable, true);
    ASSERT_EQ(exp->auto_delete, false);
    ASSERT_EQ(exp->type, XuMQ::ExchangeType::DIRECT);
    // debug(XuMQ::logger, "%s", exp->getArgs().c_str());
    ASSERT_EQ(exp->getArgs(), std::string("k2=v2&k1=v1"));
}

TEST(exchange_test, remove_test)
{
    emp->deleteExchange("exchange2");
    XuMQ::Exchange::ptr exp = emp->selectExchange("exchange2");
    ASSERT_EQ(exp.get(), nullptr);
    ASSERT_EQ(emp->exists("exchange2"), false);
}

int main(int argc, char *argv[])
{

    debug(XuMQ::logger, "empptr:%p", emp);
    debug(XuMQ::logger, "empsize:%d", emp->size());
    debug(XuMQ::logger, "exchange1:%d", emp->exists("exchange1"));
    debug(XuMQ::logger, "exchange2:%d", emp->exists("exchange2"));
    debug(XuMQ::logger, "exchange3:%d", emp->exists("exchange3"));
    debug(XuMQ::logger, "exchange4:%d", emp->exists("exchange4"));
    return 0;
}