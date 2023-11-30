#include "chat/common/ThreadPool.hpp"

namespace chat::common
{

ThreadPool::ThreadPool(uint16_t threadCount)
  : m_threadCount{threadCount},
    m_mutex{},
    m_stopping{false},
    m_conditionVariable{},
    m_threads{},
    m_jobs{}
{}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start()
{
    m_threads.reserve(m_threadCount);
    for(uint16_t i = 0; i < m_threadCount; i++)
    {
        m_threads.emplace_back(threadLoop, this);
    }
}

void ThreadPool::queue(std::function<void()> job)
{
    {
        std::unique_lock lock(m_mutex);
        m_jobs.emplace(std::move(job));
    }
    m_conditionVariable.notify_one();
}

void ThreadPool::stop()
{
    {
        std::unique_lock lock{m_mutex};
        m_stopping = true;
    }

    m_conditionVariable.notify_all();

    for(auto& thread : m_threads)
    {
        thread.join();
    }

    m_threads.clear();
}

void ThreadPool::threadLoop()
{
    while(true)
    {
        std::function<void()> job;
        {
            std::unique_lock lock(m_mutex);
            m_conditionVariable.wait(lock, [this]{return !m_jobs.empty() || m_stopping;});

            if(m_stopping)
            {
                return;
            }

            job = m_jobs.front();
            m_jobs.pop();
        }
        job();
    }
}

}
