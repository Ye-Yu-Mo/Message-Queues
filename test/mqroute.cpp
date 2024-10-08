#include "../server/route.hpp"
#include <gtest/gtest.h>
class RouteTest : public testing::Environment
{
public:
    virtual void SetUp() override {}
    virtual void TearDown() override {}
};

// TEST(route_test, legal_routing_key)
// {
//     std::string rkey1 = "news.music.pop";
//     std::string rkey2 = "news..music.pop";
//     std::string rkey3 = "news.,music.pop";
//     std::string rkey4 = "news.music_123.pop";

//     ASSERT_TRUE(XuMQ::Router::isLegalRoutingKey(rkey1));
//     ASSERT_TRUE(XuMQ::Router::isLegalRoutingKey(rkey2));
//     ASSERT_FALSE(XuMQ::Router::isLegalRoutingKey(rkey3));
//     ASSERT_TRUE(XuMQ::Router::isLegalRoutingKey(rkey4));
// }

// TEST(route_test, legal_binding_key)
// {
//     std::string bkey1 = "news.music.pop";
//     std::string bkey2 = "news..music.pop";
//     std::string bkey3 = "news.,music.pop";
//     std::string bkey4 = "news.music#.pop";
//     std::string bkey5 = "news.*.#.pop";
//     std::string bkey6 = "news.*.*.pop";
//     std::string bkey7 = "news.music_123.pop";

//     ASSERT_TRUE(XuMQ::Router::isLegalBindingKey(bkey1));
//     ASSERT_TRUE(XuMQ::Router::isLegalBindingKey(bkey2));
//     ASSERT_FALSE(XuMQ::Router::isLegalBindingKey(bkey3));
//     ASSERT_FALSE(XuMQ::Router::isLegalBindingKey(bkey4));
//     ASSERT_FALSE(XuMQ::Router::isLegalBindingKey(bkey5));
//     ASSERT_TRUE(XuMQ::Router::isLegalBindingKey(bkey6));
//     ASSERT_TRUE(XuMQ::Router::isLegalBindingKey(bkey7));
// }

TEST(route_test, routing_test)
{
    // [测试用例]
    // binding key routing key result
    // aaa                   aaa                     true
    // aaa.bbb               aaa.bbb                 true
    // aaa.bbb               aaa.bbb.ccc             false
    // aaa.bbb               aaa.ccc                 false
    // aaa.#.bbb             aaa.bbb.ccc             false
    // aaa.bbb.#             aaa.ccc.bbb             false
    // #.bbb.ccc             aaa.bbb.ccc.ddd         false
    // aaa.bbb.ccc           aaa.bbb.ccc             true
    // aaa.*                 aaa.bbb                 true
    // aaa.*.bbb             aaa.bbb.ccc             false
    // *.aaa.bbb             aaa.bbb                 false
    // #                     aaa.bbb.ccc             true
    // aaa.#                 aaa.bbb                 true
    // aaa.#                 aaa.bbb.ccc             true
    // aaa.#.ccc             aaa.ccc                 true
    // aaa.#.ccc             aaa.bbb.ccc             true
    // aaa.#.ccc             aaa.aaa.bbb.ccc         true
    // #.ccc                 ccc                     true
    // #.ccc                 aaa.bbb.ccc             true
    // aaa.#.ccc.ccc         aaa.bbb.ccc.ccc.ccc     true
    // aaa.#.bbb.*.bbb       aaa.ddd.ccc.bbb.eee.bbb true
    std::vector<std::string> bkeys = {
        "aaa",
        "aaa.bbb",
        "aaa.bbb",
        "aaa.bbb",
        "aaa.#.bbb",
        "aaa.bbb.#",
        "#.bbb.ccc",
        "aaa.bbb.ccc",
        "aaa.*",
        "aaa.*.bbb",
        "*.aaa.bbb",
        "#",
        "aaa.#",
        "aaa.#",
        "aaa.#.ccc",
        "aaa.#.ccc",
        "aaa.#.ccc",
        "#.ccc",
        "#.ccc",
        "aaa.#.ccc.ccc",
        "aaa.#.bbb.*.bbb"};

    std::vector<std::string> rkeys = {
        "aaa",
        "aaa.bbb",
        "aaa.bbb.ccc",
        "aaa.ccc",
        "aaa.bbb.ccc",
        "aaa.ccc.bbb",
        "aaa.bbb.ccc.ddd",
        "aaa.bbb.ccc",
        "aaa.bbb",
        "aaa.bbb.ccc",
        "aaa.bbb",
        "aaa.bbb.ccc",
        "aaa.bbb",
        "aaa.bbb.ccc",
        "aaa.ccc",
        "aaa.bbb.ccc",
        "aaa.aaa.bbb.ccc",
        "ccc",
        "aaa.bbb.ccc",
        "aaa.bbb.ccc.ccc.ccc",
        "aaa.ddd.ccc.bbb.eee.bbb"};

    std::vector<bool> result = {
        true,
        true,
        false,
        false,
        false,
        false,
        false,
        true,
        true,
        false,
        false,
        true,
        true,
        true,
        true,
        true,
        true,
        true,
        true,
        true,
        true
    };
    for(int i=0; i<bkeys.size();i++)
    {
        ASSERT_EQ(XuMQ::Router::route(XuMQ::ExchangeType::TOPIC, rkeys[i], bkeys[i]), result[i]);
    }
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new RouteTest);
    int res = RUN_ALL_TESTS();
    return 0;
}