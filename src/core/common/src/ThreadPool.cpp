#include "chat/common/ThreadPool.hpp"

#include "chat/common/Logging.hpp"

#include <exception>

namespace chat::common
{

ThreadPool::ThreadPool(std::size_t threadCount)
  : m_threadCount{threadCount},
    m_mutex{},
    m_stopping{false},
    m_pause{false},
    m_idleCount{0},
    m_workCondvar{},
    m_idleCondvar{},
    m_threads{},
    m_jobs{}
{
    m_threads.reserve(m_threadCount);
    for(std::size_t i = 0; i < m_threadCount; i++) {
        m_threads.emplace_back(&ThreadPool::threadLoop, this);
    }
}

ThreadPool::~ThreadPool()
{
    {
        const std::unique_lock lock{m_mutex};
        m_stopping = true;
    }
    m_workCondvar.notify_all();
    m_idleCondvar.notify_all();

    for(auto& thread : m_threads) {
        thread.join();
    }
}

void ThreadPool::queue(std::function<void()> job)
{
    {
        const std::unique_lock lock{m_mutex};
        m_jobs.emplace(std::move(job));
    }
    m_workCondvar.notify_one();
}

void ThreadPool::pause()
{
    const std::unique_lock lock{m_mutex};
    m_pause = true;
}

void ThreadPool::resume()
{
    {
        const std::unique_lock lock{m_mutex};
        m_pause = false;
    }
    m_workCondvar.notify_all();
}

void ThreadPool::waitForCompletion()
{
    std::unique_lock lock{m_mutex};
    m_idleCondvar.wait(lock, [this] {
        return (m_idleCount == m_threadCount && m_jobs.empty()) || m_stopping;
    });
}

void ThreadPool::threadLoop()
{
    while(true) {
        std::function<void()> job;
        {
            std::unique_lock lock{m_mutex};
            m_idleCount++;
            if(m_idleCount == m_threadCount && m_jobs.empty()) {
                m_idleCondvar.notify_all();
            }
            m_workCondvar.wait(lock, [this] {
                return (!m_jobs.empty() && !m_pause) || m_stopping;
            });

            if(m_stopping) {
                return;
            }

            job = m_jobs.front();
            m_jobs.pop();
            m_idleCount--;
        }

        try {
            job();
        } catch(const std::exception& exception) {
            LOG_ERROR("Exception caught: {}", exception.what());
        } catch(...) {
            LOG_ERROR("Unknown exception!");
        }
    }
}

}
