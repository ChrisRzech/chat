#include "chat/common/ThreadPool.hpp"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <thread>

namespace
{
constexpr std::chrono::milliseconds WAIT_TIME{100};

auto now()
{
    return std::chrono::system_clock::now();
}

auto getElapsedTime(std::chrono::system_clock::time_point start)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(now() - start);
}
}

TEST_CASE("Queueing 1 job into a thread pool with no threads", "[ThreadPool]")
{
    constexpr int threadCount = 0;
    chat::common::ThreadPool pool{threadCount};
    std::atomic_int count = 0;
    pool.queue([&] { count++; });
    std::this_thread::sleep_for(WAIT_TIME);
    REQUIRE(count == 0);
}

TEST_CASE("Queueing 1 job into a thread pool with 1 thread", "[ThreadPool]")
{
    constexpr int threadCount = 1;
    chat::common::ThreadPool pool{threadCount};
    std::atomic_int count = 0;
    pool.queue([&] { count++; });
    pool.waitForCompletion();
    REQUIRE(count == 1);
}

TEST_CASE("Queueing 1 job into a thread pool with 2 threads", "[ThreadPool]")
{
    constexpr int threadCount = 2;
    chat::common::ThreadPool pool{threadCount};
    std::atomic_int count = 0;
    pool.queue([&] { count++; });
    pool.waitForCompletion();
    REQUIRE(count == 1);
}

TEST_CASE("Queueing 2 jobs into a thread pool with 1 thread", "[ThreadPool]")
{
    constexpr int threadCount = 1;
    chat::common::ThreadPool pool{threadCount};

    std::atomic_int count = 0;
    auto job = [&] {
        count++;
        std::this_thread::sleep_for(WAIT_TIME);
    };

    pool.pause();
    constexpr int jobCount = 2;
    for(int i = 0; i < jobCount; i++) {
        pool.queue(job);
    }
    auto start = now();
    pool.resume();
    pool.waitForCompletion();
    auto elapsed = getElapsedTime(start);

    REQUIRE(elapsed >= (WAIT_TIME * jobCount) / threadCount);
    REQUIRE(count == 2);
}

TEST_CASE("Queueing 2 jobs into a thread pool with 2 threads", "[ThreadPool]")
{
    constexpr int threadCount = 2;
    chat::common::ThreadPool pool{threadCount};

    std::atomic_int count = 0;
    auto job = [&] {
        count++;
        std::this_thread::sleep_for(WAIT_TIME);
    };

    pool.pause();
    constexpr int jobCount = 2;
    for(int i = 0; i < jobCount; i++) {
        pool.queue(job);
    }
    auto start = now();
    pool.resume();
    pool.waitForCompletion();
    auto elapsed = getElapsedTime(start);

    REQUIRE(elapsed >= (WAIT_TIME * jobCount) / threadCount);
    REQUIRE(count == 2);
}

TEST_CASE("Waiting for empty thread pool to complete", "[ThreadPool]")
{
    constexpr int threadCount = 1;
    chat::common::ThreadPool pool{threadCount};
    auto start = now();
    pool.waitForCompletion();
    auto elapsed = getElapsedTime(start);
    REQUIRE(elapsed < WAIT_TIME);
}

TEST_CASE("Waiting for non-empty thread pool to complete", "[ThreadPool]")
{
    constexpr int threadCount = 1;
    chat::common::ThreadPool pool{threadCount};
    pool.queue([&] { std::this_thread::sleep_for(WAIT_TIME); });
    auto start = now();
    pool.waitForCompletion();
    auto elapsed = getElapsedTime(start);
    REQUIRE(elapsed >= WAIT_TIME);
}

TEST_CASE("Pausing thread pools", "[ThreadPool]")
{
    constexpr int threadCount = 1;
    chat::common::ThreadPool pool{threadCount};
    pool.pause();

    std::atomic_int count = 0;
    pool.queue([&] {
        count++;
        std::this_thread::sleep_for(WAIT_TIME);
    });
    std::this_thread::sleep_for(WAIT_TIME);
    REQUIRE(count == 0);

    auto start = now();
    pool.resume();
    pool.waitForCompletion();
    auto elapsed = getElapsedTime(start);
    REQUIRE(count == 1);
    REQUIRE(elapsed >= WAIT_TIME);
}
