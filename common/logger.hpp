#pragma once
#include "../third/Xulog/logs/Xulog.h"
// 异步日志器初始化
namespace XuMQ
{
    Xulog::Logger::ptr logger;
    class LogInit
    {
    public:
        LogInit()
        {
            std::unique_ptr<Xulog::LoggerBuilder> builder(new Xulog::GlobalLoggerBuild());
            builder->buildLoggerName("Asynclogger");
            builder->buildFormatter("[%d{%y-%m-%d|%H:%M:%S}][%f:%l][%p]%T%m%n");
            builder->buildLoggerType(Xulog::LoggerType::LOGGER_ASYNC);
            builder->buildSink<Xulog::StdoutSink>(Xulog::StdoutSink::Color::Enable);
            builder->build();
            logger = Xulog::getLogger("Asynclogger");
        }

    public:
    };
    LogInit log_init;
}
