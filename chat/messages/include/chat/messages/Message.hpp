#pragma once

#include <SFML/Network/Packet.hpp>

namespace chat::messages
{

enum Type : uint8_t
{
    Close,
    Request,
    Response
};

class Message
{
protected:
    Message(Type Type);

    virtual ~Message() = default;

public:
    [[nodiscard]] Type getType() const;

    [[nodiscard]] virtual sf::Packet toPacket() const = 0;

    virtual void fromPacket(sf::Packet& packet) = 0;

private:
    Type m_type;
};

}

/*
A message is transfered between machines to allow data communication. The sending machine forms messages that the receiving machine knows
how to interpret. Therefore, the machines must have a contract in order to properly communicate with each other.

There are different types of messages:
1. Close
2. Request
3. Response

In a proper messaging sequence on a client:
A client sends a request message and waits for a reply. If the client...
    a. doesn't get a message back in time, the client assumes the server is unreachable and closes the socket.
    b. receives a close message, the client stops sending messages to the server and closes the socket.
    c. receives a reply message, the client handles the reply message according to its logic.

In a proper messaging sequence on a server:
A server waits until a message is received. If the server...
    a. doesn't get a message after a period of time, the server assumes the client disconnected and closes the socket.
    b. receives a close message, the server stops sending messages to the client and closes the socket.
    c. recieves a request message, the server handles the request message according to its logic.

Example:
class AuthenticateRequest : public Request
{
public:
    AuthenticateRequest() = default;

    AuthenticateRequest(std::string username, std::string password)
      : m_username{std::move(username)},
        m_password{std::move(password)}
    {}

    const std::string& getUsername() const { return m_username; }
    const std::string& getPassword() const { return m_password; }

    sf::Packet toPacket() const override
    {
        //Serialize data into packet
        sf::Packet packet;
        packet << getType() << m_username << m_password;
        return packet;
    }

    bool fromPacket(sf::Packet& packet) override
    {
        //Assume that the caller extracted information from the packet to determine the appropriate message to create.
        //What is left over should be the unique data that this object holds (username and password).

        if(!(packet >> m_username))
        {
            std::cout << "Couldn't obtain username from packet" << std::endl;
            return false;
        }

        if(!(packet >> m_password))
        {
            std::cout << "Couldn't obtain password from packet" << std::endl;
            return false;
        }

        return true;
    }

private:
    std::string m_username;
    std::string m_password;
};

void sendRequest(sf::TcpSocket& socket, const Request& request)
{
    socket.send(request.toPacket());
}

std::unique_ptr<Request> receiveRequest(sf::TcpSocket& socket)
{
    sf::Packet packet;
    socket.receive(packet);

    Type messageType = Message::Type::Close;
    packet >> messageType;
    switch(messageType)
    {
        case Message::Type::Close:
        {
            //Received close request
            socket.disconnect();
            return nullptr;
        }

        case Message::Type::Reply:
        {
            //Received a reply message type but expected a request message type
            return nullptr;
        }

        case Message::Type::Request:
        {
            //Received request message type
            //If it exists, extract more information that distinguishes requests

            auto request = std::make_unique<AuthenticateRequest>();
            request->fromPacket(packet);
            return request;
        }
    }
}
*/
