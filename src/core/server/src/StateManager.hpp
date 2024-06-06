#pragma once

#include "chat/common/SynchronizedObject.hpp"

#include <condition_variable>

namespace chat::server
{

/**
 * @brief Manager for the server's state.
 */
class StateManager
{
public:
    /**
     * @brief The possible states.
     */
    enum class State
    {
        Stopped,
        Running,
        Stopping
    };

    /**
     * @brief Construct a manager.
     */
    StateManager();

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    StateManager(const StateManager& other) = delete;
    StateManager& operator=(const StateManager& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    StateManager(StateManager&& other) = delete;
    StateManager& operator=(StateManager&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the manager.
     */
    ~StateManager() = default;

    /**
     * @brief Get the current state.
     *
     * @return The current state.
     */
    State get() const;

    /**
     * @brief Transition to the provided state.
     *
     * @param value The state to transition to.
     *
     * @return True if the the current state is able to transition; otherwise,
     * false.
     */
    bool to(State value);

    /**
     * @brief Block until the current state has transitioned to the provided
     * state.
     *
     * @param value The provided state.
     */
    void waitUntil(State value);

private:
    common::SynchronizedObject<State> m_state;
    std::condition_variable m_condvar;
};

}
