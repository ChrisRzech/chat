#include "chat/client/Client.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("A client pings a server", "[Client]")
{
    const std::string hostAddress = "localhost";
    constexpr std::uint16_t port = 25565;
    chat::client::Client client{hostAddress, port};
    auto pong = client.ping();
    REQUIRE(pong.has_value());
}
