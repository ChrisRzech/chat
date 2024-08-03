#include "chat/common/SynchronizedObject.hpp"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

namespace
{
std::string getInitialValue()
{
    return "initial";
}

std::string getNewValue()
{
    return "new";
}
}

TEST_CASE("Creating a synchronized object", "[SynchronizedObject]")
{
    chat::common::SynchronizedObject<std::string> object{getInitialValue()};
    auto locked = object.lock();
    REQUIRE(locked.get() == getInitialValue());
}

TEST_CASE("Modifying a synchronized object", "[SynchronizedObject]")
{
    chat::common::SynchronizedObject<std::string> object{getInitialValue()};
    auto locked = object.lock();
    locked.get() = getNewValue();
    REQUIRE(locked.get() == getNewValue());
}

TEST_CASE("Synchronized object provides mutual exclusive access",
          "[SynchronizedObject]")
{
    chat::common::SynchronizedObject<std::string> object;

    constexpr std::chrono::milliseconds minLockTime{100};
    std::mutex mutex;
    std::atomic_bool startThread1 = false;
    bool startThread2 = false;
    std::condition_variable condvar;

    std::thread thread{[&] {
        {
            std::unique_lock lock{mutex};
            condvar.wait(lock, [&] { return startThread2; });
        }

        auto locked = object.lock();
        startThread1 = true;
        condvar.notify_one();
        std::this_thread::sleep_for(minLockTime);
    }};

    {
        std::unique_lock lock{mutex};
        startThread2 = true;
        condvar.notify_one();
        condvar.wait(lock, [&] { return startThread1.load(); });
    }

    auto start = std::chrono::system_clock::now();
    auto locked = object.lock();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - start);

    REQUIRE(elapsed >= minLockTime);

    if(thread.joinable()) {
        thread.join();
    }
}
