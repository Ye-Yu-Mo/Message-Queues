/// @file worker.hpp
/// @brief 异步工作线程模块

#pragma
#include "muduo/net/EventLoopThread.h"
#include "../common/logger.hpp"
#include "../common/helper.hpp"
#include "../common/threadpool.hpp"

namespace XuMQ
{
    /// @class AsyncWorker
    /// @brief 异步工作线程类
    class AsyncWorker
    {
    public:
        using ptr = std::shared_ptr<AsyncWorker>; ///< 异步工作线程管理句柄
        muduo::net::EventLoopThread _loopthread;  ///< muduo循环线程
        threadpool _threadpool;                   ///< 线程池
    };
}