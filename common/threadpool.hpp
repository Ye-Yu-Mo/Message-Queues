#pragma once
#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <future>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <atomic>
#include "../third/Xulog/logs/Xulog.h"

namespace XuMQ
{
    class threadpool
    {
    public:
        using ptr = std::shared_ptr<threadpool>;
        using Functor = std::function<void(void)>;
        threadpool(int thr_count = 1) : _stop(false)
        {
            for (int i = 0; i < thr_count; i++)
                _threads.emplace_back(&threadpool::entry, this);
        }
        ~threadpool()
        {
            stop();
        }
        void stop()
        {
            if (_stop == true)
                return;
            _stop = true;
            _cv.notify_all(); // 唤醒线程
            for (auto &thread : _threads)
                thread.join();
        }

        // 自动推导返回值类型
        template <typename F, typename... Args>
        auto push(F &&func, Args &&...args) -> std::future<decltype(func(args...))>
        {
            // 将传入函数封装成packaged_task任务包
            using return_type = decltype(func(args...));
            auto tmp_func = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
            auto task = std::make_shared<std::packaged_task<return_type()>>(tmp_func);
            std::future<return_type> fu = task->get_future();
            // 构造lambda表达式(捕获任务对象,函数内执行任务对象)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                // 将构造出来的匿名对象传入任务池
                _taskpool.push_back([task]()
                                    { (*task)(); });
                _cv.notify_one();
            }
            return fu;
        }

    private:
        // 线程入口函数 从任务池中取出任务执行
        void entry()
        {
            while (!_stop)
            {
                // 临时任务池
                // 避免频繁加解锁
                std::vector<Functor> tmp_taskpool;
                {
                    // 加锁
                    std::unique_lock<std::mutex> lock(_mutex);
                    // 等待任务不为空或_stop被置为1
                    _cv.wait(lock, [this]()
                             { return _stop || !_taskpool.empty(); });

                    // 取出任务进行执行
                    tmp_taskpool.swap(_taskpool);
                }
                for (auto &task : tmp_taskpool)
                {
                    task();
                }
            }
        }

    private:
        std::atomic<bool> _stop;
        std::vector<Functor> _taskpool;
        std::mutex _mutex;
        std::condition_variable _cv;
        std::vector<std::thread> _threads;
    };
}