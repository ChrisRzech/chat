#pragma once

#include <cstdint>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace chat::common
{

class ThreadPool
{
public:
    explicit ThreadPool(uint16_t threadCount);

    ThreadPool(const ThreadPool& other) = delete;

    ThreadPool& operator=(const ThreadPool& other) = delete;

    ThreadPool(ThreadPool&& other) = delete;

    ThreadPool& operator=(ThreadPool&& other) = delete;

    ~ThreadPool();

    void start();

    void queue(std::function<void()> job);

    void stop();

private:
    void threadLoop();

    uint16_t m_threadCount;
    std::mutex m_mutex;
    bool m_stopping;
    std::condition_variable m_conditionVariable;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_jobs;
};

}
