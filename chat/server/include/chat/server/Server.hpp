#pragma once

#include <cstdint>
#include <memory>

namespace chat::server
{

class Server
{
public:
    Server(uint16_t port, uint16_t maxThreadCount);

    Server(const Server& other) = delete;

    Server& operator=(const Server& other) = delete;

    Server(Server&& other) = delete;

    Server& operator=(Server&& other) = delete;

    ~Server();

    void start();

    void stop();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}
