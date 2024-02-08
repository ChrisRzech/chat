#include "chat/common/ThreadPool.hpp"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>

SCENARIO("Jobs can be queued to thread pools", "[ThreadPool]")
{
    constexpr std::chrono::milliseconds WAIT_TIME{100};
    constexpr std::chrono::milliseconds LEEWAY{10};
    std::atomic_uint32_t count = 0;
    auto simpleJob = [&]{count++;};
    auto sleepJob = [&]{count++; std::this_thread::sleep_for(WAIT_TIME);};

    GIVEN("A thread pool with no threads")
    {
        chat::common::ThreadPool pool{0};

        WHEN("A job is queued")
        {
            pool.queue(simpleJob);

            THEN("The job is not performed")
            {
                std::this_thread::sleep_for(WAIT_TIME);
                CHECK(count == 0);
            }
        }
    }

    GIVEN("A thread pool with 1 thread")
    {
        chat::common::ThreadPool pool{1};

        WHEN("A job is queued")
        {
            pool.queue(simpleJob);

            THEN("The job is performed")
            {
                pool.waitForCompletion();
                CHECK(count == 1);
            }
        }

        WHEN("Multiple jobs are queued")
        {
            pool.pause();

            constexpr uint32_t JOB_COUNT = 10;
            for(uint32_t i = 0; i < JOB_COUNT; i++)
            {
                pool.queue(sleepJob);
            }

            THEN("The jobs are performed in sequence")
            {
                auto start = std::chrono::system_clock::now();
                pool.resume();
                pool.waitForCompletion();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

                CHECK(count == JOB_COUNT);
                CHECK(elapsed >= JOB_COUNT * WAIT_TIME - LEEWAY);
            }
        }
    }

    GIVEN("A thread pool with multiple threads")
    {
        if(std::thread::hardware_concurrency() < 2)
        {
            SKIP("Not enough CPU cores");
        }

        constexpr uint16_t POOL_SIZE = 2;
        chat::common::ThreadPool pool{POOL_SIZE};

        WHEN("Multiple jobs are queued")
        {
            pool.pause();

            for(uint16_t i = 0; i < POOL_SIZE; i++)
            {
                pool.queue(sleepJob);
            }

            THEN("The jobs are performed in parallel")
            {
                auto start = std::chrono::system_clock::now();
                pool.resume();
                pool.waitForCompletion();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

                CHECK(count == POOL_SIZE);
                CHECK(elapsed <= POOL_SIZE * WAIT_TIME - LEEWAY);
            }
        }
    }
}

SCENARIO("Thread pools can be paused", "[ThreadPool]")
{
    GIVEN("A thread pool")
    {
        chat::common::ThreadPool pool{1};

        WHEN("The thread pool is paused")
        {
            pool.pause();

            AND_WHEN("A job is queued")
            {
                constexpr std::chrono::milliseconds WAIT_TIME{100};
                std::atomic_uint32_t count = 0;
                pool.queue([&]{count++;});

                THEN("The job is not performed")
                {
                    std::this_thread::sleep_for(WAIT_TIME);
                    CHECK(count == 0);
                }

                WHEN("The thread pool is resumed")
                {
                    pool.resume();

                    THEN("The job is performed")
                    {
                        pool.waitForCompletion();
                        CHECK(count == 1);
                    }
                }
            }
        }
    }
}

SCENARIO("Thread pools can notify when the job queue is empty", "[ThreadPool]")
{
    constexpr std::chrono::milliseconds LEEWAY{10};

    GIVEN("A thread pool")
    {
        chat::common::ThreadPool pool{1};
        std::atomic_uint32_t count = 0;
        pool.pause();

        AND_GIVEN("No job is queued")
        {
            WHEN("Waiting for the job queue to be empty")
            {
                THEN("There should be little to no waiting")
                {
                    auto start = std::chrono::system_clock::now();
                    pool.resume();
                    pool.waitForCompletion();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

                    CHECK(count == 0);
                    CHECK(elapsed < LEEWAY);
                }
            }
        }

        AND_GIVEN("A job is queued")
        {
            constexpr std::chrono::milliseconds WAIT_TIME{100};
            pool.queue([&]{count++; std::this_thread::sleep_for(WAIT_TIME);});

            WHEN("Waiting for the job queue to be empty")
            {
                THEN("There should be waiting time")
                {
                    auto start = std::chrono::system_clock::now();
                    pool.resume();
                    pool.waitForCompletion();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

                    CHECK(count == 1);
                    CHECK(elapsed >= WAIT_TIME - LEEWAY);
                }
            }
        }
    }
}
