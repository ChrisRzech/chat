#include "StateManager.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>

using State = chat::server::StateManager::State;

SCENARIO("Transitioning from stopped state", "[StateManager]")
{
    GIVEN("A state manager")
    {
        chat::server::StateManager state;

        THEN("The initial state is stopped")
        {
            REQUIRE(state.get() == State::Stopped);
        }

        THEN("Transitioning to a non-transitionable state fails")
        {
            REQUIRE(!state.to(State::Stopped));
            REQUIRE(!state.to(State::Stopping));
        }

        WHEN("Transitioning to the running state")
        {
            REQUIRE(state.to(State::Running));

            THEN("The state changes")
            {
                REQUIRE(state.get() == State::Running);
            }
        }
    }
}

SCENARIO("Transitioning from running state", "[StateManager]")
{
    GIVEN("A state manager that is in the running state")
    {
        chat::server::StateManager state;
        REQUIRE(state.to(State::Running));

        THEN("Transitioning to a non-transitionable state fails")
        {
            REQUIRE(!state.to(State::Stopped));
            REQUIRE(!state.to(State::Running));
        }

        WHEN("Transitioning to the stopping state")
        {
            REQUIRE(state.to(State::Stopping));

            THEN("The state changes")
            {
                REQUIRE(state.get() == State::Stopping);
            }
        }
    }
}

SCENARIO("Transitioning from stopping state", "[StateManager]")
{
    GIVEN("A state manager that is in the stopping state")
    {
        chat::server::StateManager state;
        REQUIRE(state.to(State::Running));
        REQUIRE(state.to(State::Stopping));

        THEN("Transitioning to a non-transitionable state fails")
        {
            REQUIRE(!state.to(State::Running));
            REQUIRE(!state.to(State::Stopping));
        }

        WHEN("Transitioning to the stopped state")
        {
            REQUIRE(state.to(State::Stopped));

            THEN("The state changes")
            {
                REQUIRE(state.get() == State::Stopped);
            }
        }
    }
}

SCENARIO("Waiting until a specific state", "[StateManager]")
{
    GIVEN("A state manager")
    {
        chat::server::StateManager state;

        AND_GIVEN("A thread that changes the state")
        {
            constexpr State EXPECTED_STATE = State::Running;
            constexpr std::chrono::milliseconds LEEWAY{10};
            constexpr std::chrono::milliseconds WAIT_TIME{500};
            std::thread thread{[&] {
                std::this_thread::sleep_for(WAIT_TIME);
                state.to(EXPECTED_STATE);
            }};

            WHEN("Waiting for the state to change")
            {
                auto start = std::chrono::system_clock::now();
                state.waitUntil(EXPECTED_STATE);
                auto end = std::chrono::system_clock::now();

                THEN("A notification is received")
                {
                    REQUIRE(
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            end - start) >= WAIT_TIME - LEEWAY);
                }
            }

            if(thread.joinable()) {
                thread.join();
            }
        }
    }
}
