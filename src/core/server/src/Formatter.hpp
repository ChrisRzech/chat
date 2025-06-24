#pragma once

#include <asio/error_code.hpp>
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

template<>
struct std::formatter<asio::error_code> : std::formatter<std::string>
{
    auto format(const asio::error_code& ec, std::format_context& context) const
    {
        return std::format_to(context.out(), "{} ({})", ec.message(),
                              ec.value());
    }
};
