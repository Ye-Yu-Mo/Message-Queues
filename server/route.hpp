
#pragma once
#include <iostream>
#include "../common/msg.pb.h"
#include "../common/logger.hpp"
#include "../common/helper.hpp"
namespace XuMQ
{
    /// @brief 路由管理器
    class Router
    {
    public:
        /// @brief 判断路由关键字是否合法
        /// @param routing_key 路由关键字
        /// @return 合法返回true 不合法返回false
        /// @note 合法字符 `a~z` `A~Z` `0~9` `.` `_`
        static bool isLegalRoutingKey(const std::string &routing_key)
        {
            // 判断是否有非法字符
            for (auto &ch : routing_key)
            {
                if ((ch >= 'a' && ch <= 'z') ||
                    (ch >= 'A' && ch <= 'Z') ||
                    (ch >= '0' && ch <= '9') ||
                    (ch == '_' || ch == '.'))
                    continue;
                else
                {
                    error(logger, "routing key不合法! 因为含有非法字符 %c ", ch);
                    return false;
                }
            }
            return true;
        }
        /// @brief 判断绑定关键字是否合法
        /// @param binding_key 绑定关键字
        /// @return 合法返回true 不合法返回false
        /// @note
        /// 约定字符: `0~9` `a~z` `A~Z` `_` `.` `#` `*`
        /// 注意:
        /// 1. `*` 通配符可以代替一个任意单词 例如: `news.*.footable`是合法的
        /// 2. `#` 通配符可以代替任意多个(0个或任意个)单词
        /// 3. 在两个点直接不允许出现通配符和其他字符
        /// 4. `#` 通配符不允许连续使用通配符 没有意义
        static bool isLegalBindingKey(const std::string &binding_key)
        {
            for (auto &ch : binding_key)
            {
                if ((ch >= 'a' && ch <= 'z') ||
                    (ch >= 'A' && ch <= 'Z') ||
                    (ch >= '0' && ch <= '9') ||
                    (ch == '_' || ch == '.') ||
                    (ch == '#' || ch == '*'))
                    continue;
                else
                {
                    error(logger, "binding key不合法! 因为含有非法字符 %c ", ch);
                    return false;
                }
            }
            std::vector<std::string> sub_words;
            StrHelper::split(binding_key, ".", sub_words);
            for (auto &word : sub_words)
            {
                if (word.size() > 1 &&
                    (word.find("*") != std::string::npos ||
                     word.find("#") != std::string::npos))
                {
                    error(logger, "binding key不合法! 因为字符和通配符连续出现 %s ", word.c_str());
                    return false;
                }
            }
            for (int i = 1; i < sub_words.size(); i++)
            {
                if (sub_words[i] == "#" && sub_words[i - 1] == "*")
                {
                    error(logger, "binding key不合法! 因为通配符连续出现");
                    return false;
                }
                if (sub_words[i] == "#" && sub_words[i - 1] == "#")
                {
                    error(logger, "binding key不合法! 因为通配符连续出现");
                    return false;
                }
                if (sub_words[i] == "*" && sub_words[i - 1] == "#")
                {
                    error(logger, "binding key不合法! 因为通配符连续出现");
                    return false;
                }
            }
            return true;
        }
        /// @brief 路由选择
        /// @param type 路由模式
        /// @param routing_key 路由关键字
        /// @param binding_key 绑定关键字
        /// @return 是否匹配成功 匹配成功返回true 匹配失败返回false
        static bool route(ExchangeType type, const std::string &routing_key, const std::string &binding_key)
        {
            if (type == ExchangeType::DIRECT)
                return routing_key == binding_key;
            if (type == ExchangeType::FANOUT)
                return true;

            // 动态规划进行匹配
            // 二维数组标记每次匹配的结果 通过最终数组末尾的位置来判定整体是否匹配成功
            // 初始化 dp[0][0] = 1;
            // 遇到 普通字符
            // 如果匹配成功 dp[i][j] = dp[i-1][j-1];
            // 如果匹配失败 dp[i][j] = 0;
            // 遇到 通配符 `#`
            // 初始化 bindingkey以 `#`起始时 对应行第一个字符初始化为1
            // dp[i][j] = dp[i][j-1] || dp[i-1][j] || dp[i-1][j-1];
            // 遇到 通配符 `*` 始终认为单词匹配成功
            // dp[i][j] = dp[i-1][j-1];
            std::vector<std::string> bkeys, rkeys;
            // 字符串风格 得到单词数组
            int n_bkey = StrHelper::split(binding_key, ".", bkeys);
            int n_rkey = StrHelper::split(routing_key, ".", rkeys);
            std::vector<std::vector<bool>> dp(n_bkey + 1, std::vector<bool>(n_rkey + 1, false));
            // 初始化
            dp[0][0] = true;
            // bindingkey以#起始的行第0列置为1
            for (int i = 1; i <= bkeys.size(); i++)
            {
                if (bkeys[i - 1] == "#")
                {
                    dp[i][0] = true;
                    continue;
                }
                break;
            }
            // 匹配
            for (int i = 1; i <= n_bkey; i++)
            {
                for (int j = 1; j <= n_rkey; j++)
                {
                    if (bkeys[i - 1] == rkeys[j - 1] || bkeys[i - 1] == "*")
                    {
                        dp[i][j] = dp[i - 1][j - 1];
                    }
                    else if (bkeys[i - 1] == "#")
                    {
                        dp[i][j] = dp[i - 1][j - 1] | dp[i - 1][j] | dp[i][j - 1];
                    }
                }
            }
            return dp[n_bkey][n_rkey];
        }
    };
}