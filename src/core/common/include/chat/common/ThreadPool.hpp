#pragma once

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace chat::common
{

/**
 * @brief A pool of threads waiting to run jobs.
 */
class ThreadPool
{
public:
    /**
     * @brief Construct a thread pool.
     *
     * @param threadCount The number of threads the pool will have.
     */
    explicit ThreadPool(uint16_t threadCount);

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    ThreadPool(const ThreadPool& other) = delete;
    ThreadPool& operator=(const ThreadPool& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    ThreadPool(ThreadPool&& other) = delete;
    ThreadPool& operator=(ThreadPool&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the thread pool.
     */
    ~ThreadPool();

    /**
     * @brief Add a job to the queue.
     *
     * @param job A function to run on a thread.
     */
    void queue(std::function<void()> job);

    /**
     * @brief Prevent all threads from starting jobs.
     *
     * @details If already paused, pausing again has no effect.
     */
    void pause();

    /**
     * @brief Allow all threads to start jobs.
     *
     * @details If already resumed, resuming again has no effect.
     */
    void resume();

    /**
     * @brief Block until all threads are idle and there are no more jobs to
     * run.
     */
    void waitForCompletion();

private:
    /**
     * @brief The function the threads run.
     *
     * @details This function runs jobs from the queue when available. This
     * function stops when the thread pool is being destroyed. This function
     * pauses execution when the thread pool is paused even if there are jobs
     * available. This function notifies anyone calling @c waitForCompletion()
     * when there are no more jobs available and all threads are idle.
     */
    void threadLoop();

    uint16_t m_threadCount;
    std::mutex m_mutex;
    bool m_stopping;
    bool m_pause;
    uint16_t m_idleCount;
    std::condition_variable m_workCondvar;
    std::condition_variable m_idleCondvar;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_jobs;
};

}
