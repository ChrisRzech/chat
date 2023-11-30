#pragma once

#include <SFML/Network/IpAddress.hpp>

#include <cstdint>
#include <memory>
#include <optional>

namespace chat::client
{

class Client
{
public:
    Client(sf::IpAddress host, uint16_t port);

    Client(const Client& other) = delete;

    Client& operator=(const Client& other) = delete;

    Client(Client&& other) = delete;

    Client& operator=(Client&& other) = delete;

    ~Client();

    [[nodiscard]] std::optional<std::string> ping(std::string message);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
