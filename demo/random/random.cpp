#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>
#include <atomic>

class UUIDHelper
{
public:
    /**
     * @brief 生成一个随机 UUID。
     *
     * 该方法使用随机设备和伪随机数生成器生成一个 128 位的随机数，并
     * 将其格式化为 UUID 字符串。UUID 字符串的格式为 8-4-4-4-12，其中
     * 每部分用连字符分隔。
     *
     * @return std::string 生成的 UUID 字符串。
     */
    static std::string uuid()
    {
        std::random_device rd;
        std::mt19937_64 generator(rd());                         // 生成机器随机数作为种子 使用梅森旋转算法生成伪随机数
        std::uniform_int_distribution<int> distribution(0, 255); // 范围取到0~255
        std::stringstream ss;
        for (int i = 0; i < 8; i++)
        {
            ss << std::setw(2) << std::setfill('0') << std::hex << distribution(generator); // 转为16进制字符串 不满两位自动补0
            if (i == 3 || i == 5 || i == 7)
                ss << '-';
        }
        static std::atomic<size_t> seq(1); // 定义原子类型整数
        size_t num = seq.fetch_add(1);
        for (int i = 7; i >= 0; i--)
        {
            ss << std::setw(2) << std::setfill('0') << std::hex << ((num >> i * 8) & 0xff);
            if (i == 6)
                ss << '-';
        }
        return ss.str();
    }
};
int main()
{
    for (int i = 0; i < 20; i++)
    {
        std::cout << UUIDHelper::uuid() << std::endl;
    }
    return 0;
}