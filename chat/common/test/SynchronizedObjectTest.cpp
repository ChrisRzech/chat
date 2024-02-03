#include "chat/common/SynchronizedObject.hpp"

#include "chat/common/Logging.hpp"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

SCENARIO("Synchronized objects can be modified")
{
    GIVEN("A synchronized object with an initial value")
    {
        chat::common::SynchronizedObject<std::string> object = "initial";

        THEN("The object starts with that value")
        {
            auto locked = object.lock();
            REQUIRE(locked.get() == "initial");
        }

        WHEN("The object is locked")
        {
            auto locked = object.lock();

            THEN("The object can be modified")
            {
                locked.get() = "changed";

                AND_THEN("The object's value should be changed")
                {
                    REQUIRE(locked.get() == "changed");
                }
            }
        }
    }
}

SCENARIO("Synchronized objects provide mutual exclusive access")
{
    GIVEN("A synchronized object with an initial value")
    {
        chat::common::SynchronizedObject<std::string> object = "initial";

        THEN("The object starts with that value")
        {
            auto locked = object.lock();
            REQUIRE(locked.get() == "initial");
        }

        WHEN("A thread locks the object")
        {
            constexpr std::chrono::milliseconds MIN_LOCK_TIME{2000};
            std::atomic_bool startThread1 = false;
            std::atomic_bool startThread2 = false;
            std::condition_variable condvar;

            std::thread thread
            {
                [&]
                {
                    {
                        std::mutex mutex;
                        std::unique_lock lock{mutex};
                        condvar.wait(lock, [&]{return startThread2.load();});
                    }

                    auto locked = object.lock();
                    startThread1 = true;
                    condvar.notify_one();
                    std::this_thread::sleep_for(MIN_LOCK_TIME);
                }
            };

            startThread2 = true;
            condvar.notify_one();

            THEN("Another thread cannot access the object")
            {
                {
                    std::mutex mutex;
                    std::unique_lock lock{mutex};
                    condvar.wait(lock, [&]{return startThread1.load();});
                }

                auto start = std::chrono::system_clock::now();
                auto locked = object.lock();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

                CHECK(elapsed >= MIN_LOCK_TIME);
            }

            if(thread.joinable()) {
                thread.join();
            }
        }
    }
}
