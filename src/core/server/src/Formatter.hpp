#pragma once

#include <asio/ip/tcp.hpp>

#include <format>
#include <sstream>

template<>
struct std::formatter<asio::ip::tcp::endpoint> : std::formatter<std::string>
{
    auto format(const asio::ip::tcp::endpoint& endpoint,
                std::format_context& context) const
    {
        std::ostringstream stream;
        stream << endpoint;
        return std::formatter<std::string>::format(stream.str(), context);
    }
};
