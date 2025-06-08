#include "RequestHandler.hpp"

#include "chat/common/Logging.hpp"
#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"
#include "chat/messages/request/Ping.hpp"
#include "chat/messages/response/Pong.hpp"

#include <memory>

namespace chat::server
{
std::unique_ptr<messages::Response> RequestHandler::handle(
    const messages::Request& request)
{
    LOG_DEBUG("Handling request...");

    // TODO Do not let exceptions escape this function. If an exception is
    // caught here, an internal error response should be returned.

    // TODO Add a "malformed request" request type and a "malformed request"
    // response type. If the server is unable to understand the client's
    // request, a "malformed request" request type should by provided to this
    // function. If so, this function should return a "malfored request"
    // response.
    //
    // It is debatable whether there should be a "malformed request" request
    // type at all if it is known what the response is going to be. However,
    // having this function create all responses simplifies the design.

    std::unique_ptr<messages::Response> response;
    switch(request.getType()) {
    case messages::Request::Type::Ping:
        response = handlePing(dynamic_cast<const messages::Ping&>(request));
        break;
    }

    LOG_DEBUG("Finished handling request");
    return response;
}

std::unique_ptr<messages::Response> RequestHandler::handlePing(
    [[maybe_unused]] const messages::Ping& request)
{
    return std::make_unique<messages::Pong>();
}
}
