#include "StateManager.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>

namespace
{
enum class State
{
    A,
    B,
    C
};

const chat::server::StateManager<State>::Transitions TRANSITIONS{
    {State::A, {State::B, State::C}},
    {State::B, {State::C}},
    {State::C, {State::C}}};

auto createStateManager(State initial)
{
    return chat::server::StateManager<State>{initial, TRANSITIONS};
}
}

TEST_CASE("Initial state", "[StateManager]")
{
    const auto state = createStateManager(State::A);
    REQUIRE(state.get() == State::A);
}

TEST_CASE("Transitioning from A to A", "[StateManager]")
{
    auto state = createStateManager(State::A);
    REQUIRE(!state.to(State::A));
    REQUIRE(state.get() == State::A);
}

TEST_CASE("Transitioning from A to B", "[StateManager]")
{
    auto state = createStateManager(State::A);
    REQUIRE(state.to(State::B));
    REQUIRE(state.get() == State::B);
}

TEST_CASE("Transitioning from A to C", "[StateManager]")
{
    auto state = createStateManager(State::A);
    REQUIRE(state.to(State::C));
    REQUIRE(state.get() == State::C);
}

TEST_CASE("Transitioning from B to A", "[StateManager]")
{
    auto state = createStateManager(State::B);
    REQUIRE(!state.to(State::A));
    REQUIRE(state.get() == State::B);
}

TEST_CASE("Transitioning from B to B", "[StateManager]")
{
    auto state = createStateManager(State::B);
    REQUIRE(!state.to(State::B));
    REQUIRE(state.get() == State::B);
}

TEST_CASE("Transitioning from B to C", "[StateManager]")
{
    auto state = createStateManager(State::B);
    REQUIRE(state.to(State::C));
    REQUIRE(state.get() == State::C);
}

TEST_CASE("Transitioning from C to A", "[StateManager]")
{
    auto state = createStateManager(State::C);
    REQUIRE(!state.to(State::A));
    REQUIRE(state.get() == State::C);
}

TEST_CASE("Transitioning from C to B", "[StateManager]")
{
    auto state = createStateManager(State::C);
    REQUIRE(!state.to(State::B));
    REQUIRE(state.get() == State::C);
}

TEST_CASE("Transitioning from C to C", "[StateManager]")
{
    auto state = createStateManager(State::C);
    REQUIRE(state.to(State::C));
    REQUIRE(state.get() == State::C);
}

TEST_CASE("Waiting until a specific state", "[StateManager]")
{
    auto state = createStateManager(State::A);

    constexpr auto expected = State::B;
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
