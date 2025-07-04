#include "chat/client/Client.hpp"
#include "chat/common/Port.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string>

TEST_CASE("A client pings a server", "[Client]")
{
    const std::string hostAddress = "localhost";
    constexpr chat::common::Port port{25565};
    chat::client::Client client{hostAddress, port};
    auto pong = client.ping();
    REQUIRE(pong.has_value());
}
