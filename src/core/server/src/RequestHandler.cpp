#include "RequestHandler.hpp"

#include "chat/common/Logging.hpp"

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include "chat/messages/request/Ping.hpp"

#include "chat/messages/response/Pong.hpp"

namespace chat::server
{

std::unique_ptr<messages::Response> RequestHandler::handle(
    const messages::Request& request)
{
    LOG_DEBUG << "Handling request...";

    std::unique_ptr<messages::Response> response;
    switch(request.getType()) {
    case messages::Request::Type::Ping:
        response = handlePing(static_cast<const messages::Ping&>(request));
        break;
    }

    LOG_DEBUG << "Finished handling request";
    return response;
}

std::unique_ptr<messages::Response> RequestHandler::handlePing(
    [[maybe_unused]] const messages::Ping& request)
{
    return std::make_unique<messages::Pong>();
}

}
