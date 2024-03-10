#include "chat/client/Client.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("Sending requests to a server", "[Client]")
{
    GIVEN("A server address")
    {
        const std::string hostAddress = "localhost";
        constexpr uint16_t port = 25565;

        AND_GIVEN("The client connects")
        {
            chat::client::Client client{hostAddress, port};

            WHEN("The client sends a ping request")
            {
                auto pong = client.ping();

                THEN("The server responds to the request")
                {
                    CHECK(pong.has_value());
                }
            }
        }
    }
}
