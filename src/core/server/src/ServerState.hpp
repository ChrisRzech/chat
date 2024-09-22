#pragma once

#include <map>
#include <set>

namespace chat::server
{
enum class ServerState
{
    Stopped,
    Running,
    Stopping
};

inline std::map<ServerState, std::set<ServerState>> getServerStateTransitions()
{
    return {{ServerState::Stopped, {ServerState::Running}},
            {ServerState::Running, {ServerState::Stopping}},
            {ServerState::Stopping, {ServerState::Stopped}}};
}
}
