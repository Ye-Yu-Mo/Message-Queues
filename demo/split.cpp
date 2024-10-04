#include <iostream>
#include <string>
#include <vector>

/// @brief
/// @param str
/// @param sep
/// @param result
/// @return
size_t split(const std::string &str, const std::string &sep, std::vector<std::string> &result)
{
    // 从0位置查找指定字符的位置
    // 从上次查找的位置向后继续查找指定位置
    size_t pos, idx = 0;
    while (idx < str.size())
    {
        pos = str.find(sep, idx);
        if (pos == std::string::npos) // 没找到
        {
            result.push_back(str.substr(idx));
            return result.size();
        }
        if (pos == idx) // 位置相同则数据无效
        {
            idx += sep.size();
            continue;
        }
        result.push_back(str.substr(idx, pos - idx));
        idx = pos + sep.size();
    }
    return result.size();
}

int main()
{
    std::string str = "...new....music.#.pop..";
    std::vector<std::string> arr;
    int n = split(str, ".", arr);
    for (auto &s : arr)
    {
        std::cout << s << std::endl;
    }
}