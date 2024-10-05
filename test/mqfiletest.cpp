#include "../common/helper.hpp"

int main()
{
    XuMQ::FileHelper helper("../common/logger.hpp");
    debug(XuMQ::logger, "文件是否存在:%d", helper.exists());
    debug(XuMQ::logger, "文件大小:%ld", helper.size());

    XuMQ::FileHelper t_helper("./dir/ch/tt/tmp.h");
    if (t_helper.exists() == false)
    {
        std::string path = XuMQ::FileHelper::parentDirectory("./dir/ch/tt/tmp.h");
        if (XuMQ::FileHelper(path).exists() == false)
        {
            XuMQ::FileHelper::createDirectory(path);
        }
        XuMQ::FileHelper::createFile("./dir/ch/tt/tmp.h");
    }

    std::string body;
    helper.read(body);
    debug(XuMQ::logger, "%s", body.c_str());
    t_helper.write(body);

    t_helper.rename("./dir/ch/tt/text");
    XuMQ::FileHelper::removeFile("./dir/ch/tt/text");
    XuMQ::FileHelper::removeDirectory("./dir");
    return 0;
}