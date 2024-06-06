#include "StateManager.hpp"

#include <unordered_map>
#include <unordered_set>

namespace chat::server
{

namespace
{

using Transitions = std::unordered_map<StateManager::State,
                                       std::unordered_set<StateManager::State>>;

const Transitions& getTransitions()
{
    using State = StateManager::State;
    static const Transitions transitions = {
        {State::Stopped, {State::Running}},
        {State::Running, {State::Stopping}},
        {State::Stopping, {State::Stopped}}};
    return transitions;
}

}

StateManager::StateManager()
  : m_state{State::Stopped},
    m_condvar{}
{}

StateManager::State StateManager::get() const
{
    auto locked = m_state.lock();
    return locked.get();
}

bool StateManager::to(State value)
{
    bool success = false;

    auto locked = m_state.lock();
    const auto& transitions = getTransitions();
    auto it = transitions.find(locked.get());
    if(it != transitions.end() && it->second.count(value) == 1) {
        locked.get() = value;
        m_condvar.notify_one();
        success = true;
    }

    return success;
}

void StateManager::waitUntil(State value)
{
    auto locked = m_state.lock();
    m_condvar.wait(locked.getLock(), [&] { return locked.get() == value; });
}

}
