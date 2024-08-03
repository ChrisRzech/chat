#include "StateManager.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>

using State = chat::server::StateManager::State;

TEST_CASE("Initial state is stopped", "[StateManager]")
{
    const chat::server::StateManager state;
    REQUIRE(state.get() == State::Stopped);
}

TEST_CASE("Transitioning from stopped state to stopped state", "[StateManager]")
{
    chat::server::StateManager state;
    REQUIRE(!state.to(State::Stopped));
    REQUIRE(state.get() == State::Stopped);
}

TEST_CASE("Transitioning from stopped state to running state", "[StateManager]")
{
    chat::server::StateManager state;
    REQUIRE(state.to(State::Running));
    REQUIRE(state.get() == State::Running);
}

TEST_CASE("Transitioning from stopped state to stopping state",
          "[StateManager]")
{
    chat::server::StateManager state;
    REQUIRE(!state.to(State::Stopping));
    REQUIRE(state.get() == State::Stopped);
}

TEST_CASE("Transitioning from running state to stopped state", "[StateManager]")
{
    chat::server::StateManager state;
    REQUIRE(state.to(State::Running));
    REQUIRE(!state.to(State::Stopped));
    REQUIRE(state.get() == State::Running);
}

TEST_CASE("Transitioning from running state to running state", "[StateManager]")
{
    chat::server::StateManager state;
    REQUIRE(state.to(State::Running));
    REQUIRE(!state.to(State::Running));
    REQUIRE(state.get() == State::Running);
}

TEST_CASE("Transitioning from running state to stopping state",
          "[StateManager]")
{
    chat::server::StateManager state;
    REQUIRE(state.to(State::Running));
    REQUIRE(state.to(State::Stopping));
    REQUIRE(state.get() == State::Stopping);
}

TEST_CASE("Transitioning from stopping state to stopped state",
          "[StateManager]")
{
    chat::server::StateManager state;
    REQUIRE(state.to(State::Running));
    REQUIRE(state.to(State::Stopping));
    REQUIRE(state.to(State::Stopped));
    REQUIRE(state.get() == State::Stopped);
}

TEST_CASE("Transitioning from stopping state to running state",
          "[StateManager]")
{
    chat::server::StateManager state;
    REQUIRE(state.to(State::Running));
    REQUIRE(state.to(State::Stopping));
    REQUIRE(!state.to(State::Running));
    REQUIRE(state.get() == State::Stopping);
}

TEST_CASE("Transitioning from stopping state to stopping state",
          "[StateManager]")
{
    chat::server::StateManager state;
    REQUIRE(state.to(State::Running));
    REQUIRE(state.to(State::Stopping));
    REQUIRE(!state.to(State::Stopping));
    REQUIRE(state.get() == State::Stopping);
}

TEST_CASE("Waiting until a specific state", "[StateManager]")
{
    chat::server::StateManager state;

    constexpr auto expected = State::Running;
    constexpr std::chrono::milliseconds waitTime{500};
    constexpr std::chrono::milliseconds leeway{10};
    std::thread thread{[&] {
        std::this_thread::sleep_for(waitTime);
        state.to(expected);
    }};

    auto start = std::chrono::system_clock::now();
    state.waitUntil(expected);
    auto end = std::chrono::system_clock::now();

    REQUIRE(std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start) >= waitTime - leeway);

    if(thread.joinable()) {
        thread.join();
    }
}
