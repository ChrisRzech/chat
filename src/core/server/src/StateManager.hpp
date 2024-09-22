#pragma once

#include "chat/common/SynchronizedObject.hpp"

#include <condition_variable>
#include <map>
#include <set>

namespace chat::server
{

/**
 * @brief A state manager.
 *
 * @details This is thread-safe.
 */
template<typename T>
class StateManager
{
public:
    using Transitions = std::map<T, std::set<T>>;

    /**
     * @brief Construct a manager.
     */
    StateManager(T initial, Transitions transitions)
      : m_state{initial},
        m_transitions{std::move(transitions)},
        m_condvar{}
    {}

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
    T get() const
    {
        auto locked = m_state.lock();
        return locked.get();
    }

    /**
     * @brief Transition to the provided state.
     *
     * @param value The state to transition to.
     *
     * @return True if the the current state is able to transition; otherwise,
     * false.
     */
    bool to(T value)
    {
        bool success = false;

        auto locked = m_state.lock();
        auto it = m_transitions.find(locked.get());
        if(it != m_transitions.end() && it->second.count(value) == 1) {
            locked.get() = value;
            m_condvar.notify_one();
            success = true;
        }

        return success;
    }

    /**
     * @brief Block until the current state has transitioned to the provided
     * state.
     *
     * @param value The provided state.
     */
    void waitUntil(T value)
    {
        auto locked = m_state.lock();
        m_condvar.wait(locked.getLock(), [&] { return locked.get() == value; });
    }

private:
    common::SynchronizedObject<T> m_state;
    Transitions m_transitions;
    std::condition_variable m_condvar;
};

}
