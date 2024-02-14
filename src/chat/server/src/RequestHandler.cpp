#include "RequestHandler.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

namespace chat::server
{

std::unique_ptr<chat::messages::Response> RequestHandler::handle(const chat::messages::Request& request)
{
    LOG_DEBUG << "Handling request...";

    std::unique_ptr<chat::messages::Response> response;
    switch(request.getType())
    {
    case chat::messages::Request::Type::Ping:
        response = handlePing(static_cast<const chat::messages::Ping&>(request));
        break;
    }

    LOG_DEBUG << "Finished handling request";
    return response;
}

std::unique_ptr<chat::messages::Response> RequestHandler::handlePing([[maybe_unused]] const chat::messages::Ping& request)
{
    return std::make_unique<chat::messages::Pong>();
}

}
